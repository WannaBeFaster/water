#include "../../headers.h"

#include "FourierTransform.h"

//-------------------------------------------------------------------------------------------------
void DFT::init(int N, bool norming)
{
    N_ = N;
    Norming_ = norming;
}

//-------------------------------------------------------------------------------------------------
// DFTFull
//-------------------------------------------------------------------------------------------------
void DFTFull::init(int N, bool norming)
{
    DFT::init(N, norming);

    res_.resize(N);
}

//-------------------------------------------------------------------------------------------------
void DFTFull::direct(std::vector<Complex>& data)
{
    int N = getSize();

    for(int k=0; k<N; k++)
    {
        res_[k] = Complex(0.0, 0.0);

        for(int n=0; n<N; n++)
        {
            // Sum (f(n)*exp(-2*pi*n*k/N))
            double arg = -2*glm::pi<double>()*n*k/N;

            res_[k] += data[n]*Complex::exp(arg);
        }
    }

    // copy to output buffer
    std::copy(res_.begin(), res_.end(), data.begin());
}

//-------------------------------------------------------------------------------------------------
void DFTFull::reverse(std::vector<Complex>& data)
{
    bool norming = getNorming();
    int N = getSize();

    for(int n=0; n<N; n++)
    {
        res_[n] = Complex(0.0, 0.0);

        for(int k=0; k<N; k++)
        {
            // Sum (F(k)*exp(2*pi*n*k/N))
            double arg = 2*glm::pi<double>()/N * n*k;

            res_[n] += data[k]*Complex::exp(arg);
        }

        // apply norming coeff
        if (norming)
        {
            res_[n].Re /= N;
            res_[n].Im /= N;
        }
    }

    // copy to output buffer
    std::copy(res_.begin(), res_.end(), data.begin());
}

//-------------------------------------------------------------------------------------------------
// FFT
//-------------------------------------------------------------------------------------------------
void FFT::init(int N, bool norming)
{
    DFT::init(N, norming);

    // preallocate memory
    res_.resize(N);

    int depth = log2(N);
    splits_.resize(depth);

    int Ni = N/2;
    for(auto& split: splits_)
    {
        split.even.resize(Ni);
        split.odd.resize(Ni);
        split.evenRes.resize(Ni);
        split.oddRes.resize(Ni);
        Ni /= 2;
    }

    // precalculate twiddle coeffs (depth*Ni/2)
    // Wdirect_[0] = W(0, N), W(1, N), ... W(N/2-1, N)

    Wdirect_.resize(depth);
    Wreverse_.resize(depth);
    Ni = N;
    for(int d=0; d<depth; d++)
    {
        Wdirect_[d].resize(Ni/2);
        Wreverse_[d].resize(Ni/2);
        for(int k=0; k<Ni/2; k++)
        {
            double arg = -2*glm::pi<double>()/Ni * k;
            Wdirect_[d][k] = Complex::exp(arg);
            Wreverse_[d][k] = Complex::exp(-arg);
        }
        Ni /= 2;
    }
}

//-------------------------------------------------------------------------------------------------
void FFT::direct(std::vector<Complex>& data)
{
    split(data, res_, getSize(), 0, Wdirect_);

    std::copy(res_.begin(), res_.end(), data.begin());
}

//-------------------------------------------------------------------------------------------------
void FFT::split(const std::vector<Complex>& in, std::vector<Complex>& out, int N, int level, const std::vector<std::vector<Complex>>& Warr)
{
    /*
     *  0 1 2 3 4 5 6 7 (splits[0].res)
     *        /         \
     *  0 2 4 6         1 3 5 7
     *   /  \            /   \
     * 0 4   2 6       1 5   3 7
    */

    if (N == 1)
    {
        out[0] = in[0];
        return;
    }

    // even
    std::vector<Complex>& even = splits_[level].even;
    std::vector<Complex>& evenRes = splits_[level].evenRes;

    // odd
    std::vector<Complex>& odd = splits_[level].odd;
    std::vector<Complex>& oddRes = splits_[level].oddRes;

    // split input vector to odd and even subvectors
    int i=0;
    std::copy_if(in.begin(), in.end(), even.begin(), [&i](const Complex&) {return !(i++&1);});

    i=0;
    std::copy_if(in.begin(), in.end(), odd.begin(), [&i](const Complex&) {return (i++&1);});

    //
    split(even, evenRes, N/2, level+1, Warr);
    split(odd, oddRes, N/2, level+1, Warr);

    //
    for(int k=0; k<N/2; k++)
    {
        const Complex& W = Warr[level][k];

        out[k] = evenRes[k] + W*oddRes[k];
        out[k+N/2] = evenRes[k] - W*oddRes[k];
    }
}

//-------------------------------------------------------------------------------------------------
void FFT::reverse(std::vector<Complex>& data)
{
    int N = getSize();

    split(data, res_, N, 0, Wreverse_);

    // apply norming coeff
    if (getNorming())
    {
        for(auto& n: res_)
            n /= N;
    }

    //
    std::copy(res_.begin(), res_.end(), data.begin());
}
