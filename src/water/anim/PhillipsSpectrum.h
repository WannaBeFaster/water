#pragma once

//-------------------------------------------------------------------------------------------------
class PhillipsSpectrum
{
public:
    struct Settings
    {
        // wind speed, meters/sec
        double WindSpeed;

        // wind direction, must be unit vector
        glm::vec2 WindDirection;

        // To align waves with wind's direction
        double WindAlignment;

        double A;

        // min wave size, meters
        double MinWaveLength;
    };

    void init(const Settings& settings);
    const Settings& getSettings() const {return settings_;}
    double getValue(double kx, double kz);

private:
    Settings settings_;

    // max wave size (depends on wind's speed)
    double L_;
};
