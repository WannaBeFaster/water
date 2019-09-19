#pragma once

#include "../math/Complex.h"
#include "PhillipsSpectrum.h"
#include "Animator.h"
#include "../scene/Settings.h"

class DFT;

//--------------------------------------------------------------------------------------------------
// Implements DFT-bases waves simulation (based on Tessendorf, 1999, 2004)
// CPU version.

class AnimatorDFT : public Animator
{
public:
    AnimatorDFT();

    virtual void init(const Settings& settings);
    virtual void update(double time);

protected:
    Settings settings_;

    void animateSpectrum(double time);

    void calculateHeights();
    void calculateDisplacements();
    void calculateNormalsFromDisplacement();
    void calculateNormalsFromSlope();

    int wrap(int coord, int delta);

private:
    // spectrum of initial waves state (h0(k,t)
    std::vector<std::vector<Complex>> h0_;

    // spectrum of animated waves state (F(k, t)
    std::vector<std::vector<Complex>> FH_;

    // 2 spectrums to calculate 2d displacement vector
    std::vector<std::vector<Complex>> FDx_;
    std::vector<std::vector<Complex>> FDz_;

    // Displacement map
    std::vector<std::vector<glm::vec2>> D_;
    std::vector<Complex> Resx_;
    std::vector<Complex> Resy_;

    // Slope
    std::vector<std::vector<glm::vec2>> S_;
    std::vector<std::vector<Complex>> FSx_;
    std::vector<std::vector<Complex>> FSz_;
    std::vector<Complex> Sx_;
    std::vector<Complex> Sz_;

    DFT* dft_;
    DFT* dftS_;
    DFT* dftD_;

    PhillipsSpectrum spectrum_;

    std::vector<Complex> Res_;
};
