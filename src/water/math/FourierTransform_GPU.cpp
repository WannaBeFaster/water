#include "../../headers.h"

#include "FourierTransform_GPU.h"

//-------------------------------------------------------------------------------------------------
// FFTGPU
//-------------------------------------------------------------------------------------------------
void FFTGPU::init(int N)
{
    N_ = N;
    currentDest_ = 0;

    fft_[0].init(N_, N_);
    fft_[1].init(N_, N_);

    prg_.load("data/shaders/Passthrough_vertex.glsl", "data/shaders/FFT_fragment.glsl");
    fft_[0].setProgram(&prg_);
    fft_[1].setProgram(&prg_);
}

//-------------------------------------------------------------------------------------------------
void FFTGPU::reverse(GLuint txIn, FrameBuffer* dest)
{
    // do 2D transform as 2 1D transforms (rows first, then columns)

    subReverse(txIn, nullptr, true);
    subReverse(getCurrentTarget()->getTextureID(), dest, false);
}

//-------------------------------------------------------------------------------------------------
void FFTGPU::subReverse(GLuint txIn, FrameBuffer* dest, bool horiz)
{
    int numIterations = log2(N_);

    for(int i=1; i<=numIterations; i++)
    {
        swapTargets();
        FrameBuffer* in = getCurrentSource();
        FrameBuffer* out = getCurrentTarget();

        GLuint inputTexture = in->getTextureID();

        if (i == 1)
        {
            inputTexture = txIn;
        }

        if (i == numIterations && dest != nullptr)
        {
            out = dest;
        }

        out->setProgram(&prg_);
        auto setUniforms = [&](Program& prg)
        {
            prg.setUniformTexture("u_source", inputTexture, GL_TEXTURE0);
            prg.setUniform1i("u_size", N_);
            prg.setUniform1i("u_iteration", i);
            prg.setUniform1i("u_horiz", horiz ? 1 : 0);
        };
        out->render(setUniforms);
    }
}

//-------------------------------------------------------------------------------------------------
FrameBuffer* FFTGPU::getCurrentSource()
{
    return &fft_[1 - currentDest_];
}

//-------------------------------------------------------------------------------------------------
FrameBuffer* FFTGPU::getCurrentTarget()
{
    return &fft_[currentDest_];
}

//-------------------------------------------------------------------------------------------------
void FFTGPU::swapTargets()
{
    currentDest_ = 1 - currentDest_;
}
