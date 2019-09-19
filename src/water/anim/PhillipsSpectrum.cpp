#include "../../headers.h"

#include "PhillipsSpectrum.h"

//-------------------------------------------------------------------------------------------------
void PhillipsSpectrum::init(const Settings& settings)
{
    settings_ = settings;

    // длина волны при заданной скорости ветра
    L_ = settings_.WindSpeed *settings_.WindSpeed/9.81;
}

//-------------------------------------------------------------------------------------------------
double PhillipsSpectrum::getValue(double kx, double kz)
{
    glm::vec2 K(kx, kz);
    double k = glm::length(K);

    // квадрат модуля вектора k
    double k2 = k*k;

    // именно точное сравнение
    if (k2 == 0.0)
        return 0.0;

    double v = settings_.A*exp(-1.0 / (k2*L_*L_)) / (k2*k2);

    // учтем направление ветра. Это отсечет волны, которые идут перпендикулярно направлению ветра.
    auto K1 = glm::normalize(K);
    double w = pow(glm::dot(K1, settings_.WindDirection), settings_.WindAlignment);
    v *= w;

    // - еще нужно отсекать слишком маленькие волны
    v *= exp(-k2*settings_.MinWaveLength*settings_.MinWaveLength);

    return v;
}
