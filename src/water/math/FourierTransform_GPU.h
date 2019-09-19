#pragma once

#include "Complex.h"
#include "../render/FrameBuffer.h"
#include "../render/Program.h"

//-------------------------------------------------------------------------------------------------
// Fast FFT on GPU (Stockham)

class FFTGPU
{
public:
    virtual void init(int N);
    virtual void reverse(GLuint txIn, FrameBuffer* dest);

    void subReverse(GLuint txIn, FrameBuffer* dest, bool horiz);

    FrameBuffer* getCurrentSource();
    FrameBuffer* getCurrentTarget();
    void swapTargets();

private:
    int N_;
    int currentDest_;

    FrameBuffer fft_[2];
    Program prg_;
};
