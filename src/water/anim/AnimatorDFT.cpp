#include "../../headers.h"

#include "../math/FourierTransform.h"
#include "../scene/Patch.h"
#include "Utils.h"
#include "AnimatorDFT.h"


//-------------------------------------------------------------------------------------------------
AnimatorDFT::AnimatorDFT()
{
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::init(const Settings& settings)
{
    settings_ = settings;

    int N = settings_.GridResolution;
    double L = settings_.Size;

    PhillipsSpectrum::Settings ps;
    ps.A = settings.A;
    ps.WindSpeed = settings.WindSpeed;
    ps.WindAlignment = settings.WindAlignment;
    ps.MinWaveLength = settings.MinWaveLength;
    ps.WindDirection = settings.WindDirection;
    spectrum_.init(ps);

    static FFT fft;
    static FFT fftS;
    static FFT fftD;

    /*
    static DFTFull fft;
    static DFTFull fftS;
    static DFTFull fftD;
    */

    fft.init(N, false);
    fftD.init(N, false);
    fftS.init(N, false);

    dft_ = &fft;
    dftS_ = &fftS;
    dftD_ = &fftD;

    // Initial height spectrum
    h0_.clear();
    h0_.resize(N);
    for(auto& row: h0_)
    {
        row.clear();
        row.resize(N);
    }

    // Animated height spectrum
    FH_.clear();
    FH_.resize(N);
    for (auto& row: FH_)
    {
        row.clear();
        row.resize(N);
    }

    // Displacement spectrum
    FDx_.clear();
    FDx_.resize(N);
    for (auto& row: FDx_)
    {
        row.clear();
        row.resize(N);
    }
    FDz_.clear();
    FDz_.resize(N);
    for (auto& row: FDz_)
    {
        row.clear();
        row.resize(N);
    }

    //---------------------
    // Slope spectrum
    FSx_.clear();
    FSx_.resize(N);
    for (auto& row: FSx_)
    {
        row.clear();
        row.resize(N);
    }
    FSz_.clear();
    FSz_.resize(N);
    for (auto& row: FSz_)
    {
        row.clear();
        row.resize(N);
    }
    Sx_.clear();
    Sx_.resize(N);
    Sz_.clear();
    Sz_.resize(N);

    // allocate memory for the result
    Res_.clear();
    Res_.resize(N);
    Resx_.resize(N);
    Resy_.resize(N);

    // displacement
    D_.clear();
    D_.resize(N);
    for (auto& row: D_)
    {
        row.clear();
        row.resize(N);
    }

    // slope
    S_.clear();
    S_.resize(N);
    for (auto& row: S_)
    {
        row.clear();
        row.resize(N);
    }

    // k - 2D vector
    // kx = 2*pi*n/Lx, kz = 2*pi*m/Ly

    // Calculate Phillips spectrum h0(k)
    // Tessendorf (42)

    std::default_random_engine gen(time(0));
    std::normal_distribution<double> d(0.0, 1.0);

    for(int m=-N/2, mi=0; m<N/2; m++, mi++)
    {
        for(int n=-N/2, ni=0; n<N/2; n++, ni++)
        {
            int nn = (ni < N/2) ? ni : ni - N;
            int mm = (mi < N/2) ? mi : mi - N;
            double ph = spectrum_.getValue(2*glm::pi<double>()*nn/L, 2*glm::pi<double>()*mm/L);

            double e1 = d(gen);
            double e2 = d(gen);

            h0_[mi][ni].Re = e1 * sqrt(ph/2);
            h0_[mi][ni].Im = e2 * sqrt(ph/2);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::animateSpectrum(double time)
{
    double L = settings_.Size;
    int N = settings_.GridResolution;

    double coeff = 2*glm::pi<double>()/L;

    for(int km=-N/2, mi=0; km<N/2; km++, mi++)
    {
        for(int kn=-N/2, ni=0; kn<N/2; kn++, ni++)
        {
            // определяем очередной вектор пространственных частот k = (kx, kz)
            int nn = (ni < N/2) ? ni : ni - N;
            int mm = (mi < N/2) ? mi : mi - N;
            double kx = coeff*nn;
            double kz = coeff*mm;

            double k = sqrt(kx*kx + kz*kz);

            // deep ocean
            double wt = sqrt(9.81*k)*time;

            // F(k,t) = h0(k)*exp(iw(k)t) + h0*(-k)*exp(-iw(k)t)

            // h0*(-k) - conjugate for h0(-k)
            const Complex& h0 = h0_[mi][ni];
            Complex h = h0*Complex::exp(wt);

            int mi2 = N - mi;
            int ni2 = N - ni;
            if (mi2 != N && ni2 != N)
            {              
                const Complex& h0_negK = h0_[mi2][ni2];
                Complex h02(h0_negK.Re, -h0_negK.Im);

                h = h + h02*Complex::exp(-wt);
            }

            if (k == 0.0)
            {
                FH_[mi][ni] = Complex(0.0, 0.0);
                FDx_[mi][ni] = Complex(0.0, 0.0);
                FDz_[mi][ni] = Complex(0.0, 0.0);
            }
            else
            {
                FH_[mi][ni] = h;

                // displacement spectrum
                Complex hX = Complex(0.0, -1.0)*(h * kx/k)*settings_.Choppiness;
                FDx_[mi][ni] = hX;

                Complex hZ = Complex(0.0, -1.0)*(h * kz/k)*settings_.Choppiness;
                FDz_[mi][ni] = hZ;

                // slope spectrum
                Complex sx = Complex(0.0, 1.0)*h*kx;
                FSx_[mi][ni] = sx;
                Complex sz = Complex(0.0, 1.0)*h*kz;
                FSz_[mi][ni] = sz;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::calculateHeights()
{
    int N = settings_.GridResolution;

    // To get wave heights we need to perform 2D IDFT on animated spectrum.
    // h(x,t) = RDFT(F(k,t)) = Sum(F(k,t)*exp(ikx)

    // 2D FFT is 2 1D FFTs

    for(int y=0; y<N; y++)
    {
        dft_->reverse(FH_[y]);
    }

    auto& vertices = patch_->getVertices();

    for(int x=0; x<N; x++)
    {
        for(int i=0; i<N; i++)
        {
            Res_[i] = FH_[i][x];
        }

        // We get wave heights in Res_[i].Re after this
        dft_->reverse(Res_);

        for(int a=0; a<N; a++)
        {
            vertices[a*N + x].pos.y = Res_[a].Re;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::calculateDisplacements()
{
    int N = settings_.GridResolution;

    // D(x,t) = Sum(-i*|k|*F(k,t)*exp(ikx))

    for(int y=0; y<N; y++)
    {
        dftD_->reverse(FDx_[y]);
        dftD_->reverse(FDz_[y]);
    }

    auto& vertices = patch_->getVertices();
    double L = settings_.Size;

    for(int x=0; x<N; x++)
    {
        for(int i=0; i<N; i++)
        {
            Resx_[i] = FDx_[i][x];
            Resy_[i] = FDz_[i][x];
        }

        // после этого преобразования мы получаем вектор D
        dftD_->reverse(Resx_);
        dftD_->reverse(Resy_);

        double f = settings_.DisplacementFactor;

        for(int a=0; a<N; a++)
        {
            double dx = Resx_[a].Re;
            double dz = Resy_[a].Re;

            D_[a][x].x = dx;
            D_[a][x].y = dz;

            double x0 = x * L/N;
            double z0 = -a * L/N;

            vertices[a*N + x].pos.x = x0 + f*dx;
            vertices[a*N + x].pos.z = z0 + f*dz;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::calculateNormalsFromSlope()
{
    int N = settings_.GridResolution;

    for(int y=0; y<N; y++)
    {
        dftS_->reverse(FSx_[y]);
        dftS_->reverse(FSz_[y]);
    }

    auto& vertices = patch_->getVertices();

    for(int x=0; x<N; x++)
    {
        for(int i=0; i<N; i++)
        {
            Sx_[i] = FSx_[i][x];
            Sz_[i] = FSz_[i][x];
        }

        // после этого преобразования мы получаем вектор slope
        dftS_->reverse(Sx_);
        dftS_->reverse(Sz_);

        for(int a=0; a<N; a++)
        {
            double sx = Sx_[a].Re;
            double sz = Sz_[a].Re;

            S_[a][x].x = sx;
            S_[a][x].y = sz;
            glm::vec3 n = glm::vec3(0, 1, 0) - glm::vec3(sx, 0.0, sz);
            double k = sqrt(1 + sx*sx + sz*sz);

            vertices[a*N + x].normal.x = n.x/k;
            vertices[a*N + x].normal.y = n.y/k;
            vertices[a*N + x].normal.z = n.z/k;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorDFT::update(double time)
{
    bool multiThreading = false;

    // анимируем спектр
    animateSpectrum(time);

    if (multiThreading)
    {
        std::thread t1(&AnimatorDFT::calculateHeights, this);
        //std::thread t2(&AnimatorRDFT::calculateDisplacements, this);
        //std::thread t3(&AnimatorRDFT::calculateNormalsFromSlope, this);

        t1.join();
        //t2.join();
        //t3.join();
    }
    else
    {
        // определяем высоты волн
        calculateHeights();

        // определяем смещения для заострения волн
        calculateDisplacements();

        // считаем поле векторов slope и нормали
        //calculateNormalsFromSlope();

        //calculateNormalsFromDisplacement();
    }
    generateNormals();

    //Utils::saveHeightmap(patch_->getVertices(), settings_.N, "heightmap.bmp");
    //Utils::saveDisplacementMap(D_, "Dx.bmp", "Dz.bmp");
    //Utils::saveDisplacementMap(S_, "Sx.bmp", "Sz.bmp");
}
