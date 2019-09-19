#pragma once

#include "Complex.h"


//-------------------------------------------------------------------------------------------------
// Base class for 1D DFT

class DFT
{
public:
    virtual void init(int N, bool norming);

    // N - transform size.
    // data - [in] Fourie, [out] - restored signal

    virtual void direct(std::vector<Complex>& data) = 0;
    virtual void reverse(std::vector<Complex>& data) = 0;

    void setNorming(bool norming) {Norming_ = norming;}
    bool getNorming() const {return Norming_;}

    int getSize() const {return N_;}

private:
    int N_;
    bool Norming_;
};

//-------------------------------------------------------------------------------------------------
// DFT, not fast

class DFTFull : public DFT
{
public:
    virtual void init(int N, bool norming);
    virtual void direct(std::vector<Complex>& data);
    virtual void reverse(std::vector<Complex>& data);

protected:
    std::vector<Complex> res_;
};

//-------------------------------------------------------------------------------------------------
// Fast DFT

class FFT : public DFT
{
public:
    virtual void init(int N, bool norming);
    virtual void direct(std::vector<Complex>& data);
    virtual void reverse(std::vector<Complex>& data);

protected:
    void split(const std::vector<Complex>& in, std::vector<Complex>& out, int N, int level, const std::vector<std::vector<Complex>>& Warr);

    struct SplitData
    {
        // vectors for split, size N/2
        std::vector<Complex> even;
        std::vector<Complex> odd;

        // N/2
        std::vector<Complex> evenRes;
        std::vector<Complex> oddRes;
    };
    std::vector<SplitData> splits_;

    std::vector<Complex> res_;

    // precalculated twiddle coeffs
    std::vector<std::vector<Complex>> Wdirect_;
    std::vector<std::vector<Complex>> Wreverse_;
};
