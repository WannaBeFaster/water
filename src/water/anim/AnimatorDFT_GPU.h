#pragma once 

#include "../render/Program.h"
#include "../render/ScreenRectangle.h"
#include "../render/FrameBuffer.h"
#include "../render/Texture.h"
#include "../math/FourierTransform_GPU.h"
#include "../scene/Settings.h"

#include "Animator.h"

//--------------------------------------------------------------------------------------------------
// Implements DFT-bases waves simulation (based on Tessendorf, 1999, 2004)
// GPU version.

class AnimatorRDFT_GPU : public Animator
{
public:
    virtual void init(const Settings& settings);
    virtual void update(double time);
    virtual void draw();
    virtual void getProgramPaths(std::string& vertex, std::string& fragment);
    virtual void setAdditionalUniforms(Program& prg);

protected:
    void initSpectrum();
    void makeRandomTexture();

    void initScreenRects();

    void animateSpectrums(double time);
    void drawScreenRects();
    void performFFTs();
    void performFFT(FrameBuffer* src, FrameBuffer* dest);
    void renderNormalMap();

private:
    Settings settings_;

    // screen rectangles
    Program programRect_;   
    ScreenRectangle rectH0_;
    ScreenRectangle rectHkt_;
    ScreenRectangle rectFS_;

    ScreenRectangle rectSlope_;
    //Program prgHeightmap_;

    ScreenRectangle rectFD_;
    ScreenRectangle rectDisp_;

    ScreenRectangle rectHeights_;
    Program prgHeightmap_;

    FrameBuffer h0_;
    Program initSpectrum_;

    FrameBuffer hkt_;
    Program animateHeightSpectrum_;
    FrameBuffer h_;

    FrameBuffer Fslope_;
    Program animateSSpectrum_;
    FrameBuffer slope_;

    FrameBuffer Fdisp_;
    Program animateDSpectrum_;
    FrameBuffer disp_;

    //
    FrameBuffer normalMap_;
    Program prgNormalMap_;

    //
    FFTGPU fft_;

    Texture random_;
};
