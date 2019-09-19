#include "../../headers.h"

#include "../scene/Patch.h"
#include "../render/Program.h"
#include "../render/ScreenRectangle.h"
#include "../render/FrameBuffer.h"
#include "../math/FourierTransform.h"
#include "PhillipsSpectrum.h"
#include "Utils.h"

#include "AnimatorDFT_GPU.h"

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::init(const Settings& settings)
{
    settings_ = settings;

    //
    initSpectrum();
    initScreenRects();

    int N = settings.GridResolution;

    // init fft buffers
    fft_.init(N);

    // init height buffer
    h_.init(N, N);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::getProgramPaths(std::string& vertex, std::string& fragment)
{
    vertex = "data/shaders/WaterGPU_vertex.glsl";
    fragment = "data/shaders/WaterGPU_fragment.glsl";
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::setAdditionalUniforms(Program& prg)
{
    prg.setUniformTexture("u_heights", h_.getTextureID(), GL_TEXTURE1);
    prg.setUniformTexture("u_normals", normalMap_.getTextureID(), GL_TEXTURE2);
    prg.setUniformTexture("u_disp", disp_.getTextureID(), GL_TEXTURE3);

    prg.setUniform1i("u_size", settings_.GridResolution);
    prg.setUniform1f("u_worldSize", settings_.GridWorldSize);
    prg.setUniform1f("u_dispFactor", settings_.DisplacementFactor);
    prg.setUniformVec3("u_colorOcean", settings_.OceanColor);
    prg.setUniformVec3("u_colorLight", settings_.LightColor);
    prg.setUniformVec3("u_colorSky", settings_.SkyColor);
    prg.setUniformVec3("u_lightDir", settings_.LightDir);
    prg.setUniform1f("u_PhongFactor", settings_.PhongFactor);
    prg.setUniform1i("u_renderSky", settings_.RenderSky);

    prg.setUniform1f("u_Kr", settings_.Kr);
    prg.setUniform1f("u_Kt", settings_.Kt);
    prg.setUniform1f("u_Ks", settings_.Ks);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::initSpectrum()
{
    int N = settings_.GridResolution;

    // setup frame buffer for initial spectrum
    h0_.init(N, N);

    // render initial spectrum
    initSpectrum_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/InitSpectrum_fragment.glsl");
    h0_.setProgram(&initSpectrum_);

    makeRandomTexture();

    auto uniforms = [&](Program& prg)
    {
        prg.setUniform1i("u_usePhillips", settings_.UsePhillips ? 1 : 0);
        prg.setUniformTexture("u_random", random_.getID(), GL_TEXTURE0);
        prg.setUniform1f("u_physSize", settings_.Size);
        prg.setUniform1i("u_size", settings_.GridResolution);
        prg.setUniform1f("u_A", settings_.A);
        prg.setUniform1f("u_V", settings_.WindSpeed);
        prg.setUniform1f("u_windAlignment", settings_.WindAlignment);
        prg.setUniform1f("u_minWaveLength", settings_.MinWaveLength);
        prg.setUniformVec2("u_windDir", settings_.WindDirection);
    };
    h0_.render(uniforms);

    // frame buffer for animated heights spectrum
    hkt_.init(N, N);
    animateHeightSpectrum_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/AnimateHeightSpectrum_fragment.glsl");
    hkt_.setProgram(&animateHeightSpectrum_);

    // frame buffer for animated slope spectrum
    Fslope_.init(N, N);
    animateSSpectrum_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/AnimateSSpectrum_fragment.glsl");
    Fslope_.setProgram(&animateSSpectrum_);
    slope_.init(N, N);

    // frame buffer for animated displacement spectrum
    Fdisp_.init(N, N);
    animateDSpectrum_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/AnimateDSpectrum_fragment.glsl");
    Fdisp_.setProgram(&animateDSpectrum_);
    disp_.init(N, N);

    // normal map frame buffer
    normalMap_.init(N, N);
    prgNormalMap_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/NormalMap_fragment.glsl");
    normalMap_.setProgram(&prgNormalMap_);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::makeRandomTexture()
{
    // generate random texture
    int N = settings_.GridResolution;

    struct RGBA
    {
        float r, g, b, a;
    };
    std::vector<RGBA> data;
    data.resize(N*N);

    // random numbers source
    std::default_random_engine gen(time(0));
    std::normal_distribution<double> d(0.0, 1.0);

    for (int i=0; i<N*N; i++)
    {
        // for Phillips spectrum
        data[i].r = d(gen);
        data[i].g = d(gen);
        //data[i].r = 0.0;
        //data[i].g = 0.0;

        // initial phase
        data[i].b = 2*glm::pi<double>()*(d(gen) + 1)/2;

        data[i].a = 0.0f;
    }

    random_.create();

    glBindTexture(GL_TEXTURE_2D, random_.getID());

    // allocate memory
    glTexImage2D(GL_TEXTURE_2D,
                 0,  // no mips, level 0
                 GL_RGBA32F,
                 N,
                 N,
                 0,
                 GL_RGBA,
                 GL_FLOAT,
                 data.data());

    // no need for filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::initScreenRects()
{
    int N = 128;

    programRect_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/ScreenRect_fragment.glsl");
    GLuint id = programRect_.getID();

    prgHeightmap_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/Heightmap_fragment.glsl");
    GLuint idHeightmap = prgHeightmap_.getID();

    struct Viewport
    {
        GLint   x;
        GLint   y;
        GLsizei width;
        GLsizei height;
    } vp;
    glGetIntegerv(GL_VIEWPORT, (GLint*)&vp);

    rectH0_.init(0, 0, N, N, vp.width, vp.height);
    rectH0_.setProgram(id);

    rectHkt_.init(N, 0, N, N, vp.width, vp.height);
    rectHkt_.setProgram(id);

    rectHeights_.init(N, N, N, N, vp.width, vp.height);
    //rectHeights_.setProgram(id);
    rectHeights_.setProgram(idHeightmap);

    rectFS_.init(2*N, 0, N, N, vp.width, vp.height);
    rectFS_.setProgram(id);
    rectSlope_.init(2*N, N, N, N, vp.width, vp.height);
    //rectSlope_.setProgram(id);
    rectSlope_.setProgram(idHeightmap);

    rectFD_.init(3*N, 0, N, N, vp.width, vp.height);
    rectFD_.setProgram(id);
    rectDisp_.init(3*N, N, N, N, vp.width, vp.height);
    rectDisp_.setProgram(id);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::update(double time)
{
    animateSpectrums(time);
    performFFTs();
    renderNormalMap();
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::renderNormalMap()
{
    auto setUniforms = [&](Program& prg)
    {
        prg.setUniformTexture("u_slope", slope_.getTextureID(), GL_TEXTURE0);
        prg.setUniform1i("u_size", settings_.GridResolution);
        prg.setUniform1f("u_physSize", settings_.GridWorldSize);
        prg.setUniform1f("u_smoothNormals", settings_.SmoothNormals);
    };
    normalMap_.render(setUniforms);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::draw()
{
    //drawScreenRects();
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::animateSpectrums(double time)
{
    // heights spectrum
    {
        auto setUniforms = [&](Program& prg)
        {
            prg.setUniformTexture("u_h0", h0_.getTextureID(), GL_TEXTURE0);
            prg.setUniformTexture("u_random", random_.getID(), GL_TEXTURE1);
            prg.setUniform1f("u_time", time);
            prg.setUniform1f("u_physSize", settings_.Size);
            prg.setUniform1i("u_size", settings_.GridResolution);
        };

        hkt_.render(setUniforms);
    }

    // disp & slope spectrums
    {
        auto setUniforms = [&](Program& prg)
        {
            prg.setUniformTexture("u_hkt", hkt_.getTextureID(), GL_TEXTURE0);
            prg.setUniform1f("u_physSize", settings_.Size);
            prg.setUniform1i("u_size", settings_.GridResolution);
        };
        Fslope_.render(setUniforms);
    }
    {
        auto setUniforms = [&](Program& prg)
        {
            prg.setUniformTexture("u_hkt", hkt_.getTextureID(), GL_TEXTURE0);
            prg.setUniform1f("u_physSize", settings_.Size);
            prg.setUniform1i("u_size", settings_.GridResolution);
            prg.setUniform1f("u_choppiness", settings_.Choppiness);
        };
        Fdisp_.render(setUniforms);
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::performFFTs()
{
    // we have to transform 5 spectrums:
    // - 1 for heights
    fft_.reverse(hkt_.getTextureID(), &h_);

    // - 2 for slope vector
    fft_.reverse(Fslope_.getTextureID(), &slope_);

    // - 2 for displacement vector
    fft_.reverse(Fdisp_.getTextureID(), &disp_);
}

//-------------------------------------------------------------------------------------------------
void AnimatorRDFT_GPU::drawScreenRects()
{
    int size = 128;

    //rectH0_.setTexture(h0_.getTexture(), size);
    rectH0_.setTexture(normalMap_.getTextureID(), size);
    rectH0_.draw();

    rectHkt_.setTexture(hkt_.getTextureID(), size);
    rectHkt_.draw();

    rectHeights_.setTexture(h_.getTextureID(), size);
    rectHeights_.draw();

    rectFS_.setTexture(Fslope_.getTextureID(), size);
    rectFS_.draw();

    rectSlope_.setTexture(slope_.getTextureID(), size);
    rectSlope_.draw();

    rectFD_.setTexture(Fdisp_.getTextureID(), size);
    rectFD_.draw();

    rectDisp_.setTexture(disp_.getTextureID(), size);
    rectDisp_.draw();
}
