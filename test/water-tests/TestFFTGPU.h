#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../src/water/math/FourierTransform.h"
#include "../../src/water/math/FourierTransform_GPU.h"

using namespace testing;

GLuint makeTexture(int size, float* data)
{
    GLuint tx;

    glGenTextures(1, &tx);
    glBindTexture(GL_TEXTURE_2D, tx);

    //
    glTexImage2D(GL_TEXTURE_2D,
                 0,  // no mips, level 0
                 GL_RGBA32F,
                 //GL_RGB,
                 size,
                 size,
                 0,
                 GL_RGBA,
                 //GL_RGB,
                 GL_FLOAT,
                 //GL_UNSIGNED_BYTE,
                 data);

    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return tx;
}

TEST(FFTGPUTests, Test1)
{
    //--------------------------------------------------------
    // Prepare test data

    int N = 128;
    std::vector<Complex> data;
    data.reserve(N);
    std::vector<Complex> initial;
    initial.reserve(N);

    for(int n=0; n<N; n++)
    {
        // f(x) = x
        double x = ((double)n)/N;
        data.push_back(Complex(x, 0.0));
        initial.push_back(Complex(x, 0.0));
    }

    //--------------------------------------------------------
    //

    DFTFull ft;
    ft.init(N, true);
    ft.direct(data);

    //
    if( !glfwInit() )
        throw std::runtime_error("Failed to initialize GLFW\n");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    auto window_ = glfwCreateWindow( 128, 128, "test", NULL, NULL);
    if( window_ == nullptr )
    {
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    }

    glfwMakeContextCurrent(window_);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW\n");
    }

    // prepare texture: fill it with data
    struct RGBA
    {
        float r, g, b, a;
    };
    RGBA txData[N][N];
    for(int n=0; n<N; n++)
    {
        txData[9][n].r = data[n].Re;
        txData[9][n].g = data[n].Im;
        txData[9][n].b = txData[0][n].a = 0.0f;
    }
    GLuint tx = makeTexture(N, (float*)txData);

    FFTGPU fftgpu;
    fftgpu.init(N);

    FrameBuffer target;
    target.init(N, N);

    // perform mega transformation
    fftgpu.subReverse(tx, &target, true);

    // check rendered texture
    glBindFramebuffer(GL_FRAMEBUFFER, target.getID());
    RGBA color;
    for(int n=0; n<N; n++)
    {
        glReadPixels(n, 9, 1, 1, GL_RGBA, GL_FLOAT, &color);
        ASSERT_NEAR(color.r/N, initial[n].Re, 1e-4);
    }

    //----------------------------
    glDeleteTextures(1, &tx);
    glfwTerminate();
}
