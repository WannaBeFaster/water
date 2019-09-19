#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "../../src/water/math/FourierTransform.h"

using namespace testing;

void testDFT(DFT* ft)
{
    int N = 64;
    std::vector<Complex> data;
    data.reserve(N);
    std::vector<Complex> initial;
    initial.reserve(N);

    ft->init(N, true);

    for(int n=0; n<N; n++)
    {
        // f(x) = x
        double x = ((double)n)/N;
        data.push_back(Complex(x, 0.0));
        initial.push_back(Complex(x, 0.0));
    }

    //
    ft->direct(data);
    ft->reverse(data);

    for(int n=0; n<N; n++)
    {
        ASSERT_NEAR(initial[n].Re, data[n].Re, 1e-6);
        ASSERT_NEAR(initial[n].Im, data[n].Im, 1e-6);
    }
}

TEST(FourieTransformTests, DiscreteFull)
{
    DFTFull dft;
    testDFT(&dft);
}

TEST(FourieTransformTests, DiscreteFast)
{
    FFT dft;
    testDFT(&dft);
}
