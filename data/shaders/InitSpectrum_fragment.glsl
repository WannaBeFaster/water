#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform sampler2D u_random;
uniform int u_size;
uniform float u_physSize;

uniform int u_usePhillips;

uniform float u_V;
uniform vec2 u_windDir;

// Phillips
uniform float u_A;
uniform float u_windAlignment;
uniform float u_minWaveLength;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec2 v_uv;

//-------------------------------------------------------------------------------------------------
// Constants
const float pi = 3.14159265358;
const float g = 9.81;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
// Phillips spectrum
float getPhillips(float kx, float kz)
{
    float V = u_V;
    float Lwave = V*V/g;

    vec2 K = vec2(kx, kz);
    float k = length(K);
    float k2 = k*k;

    if (k2 < 1e-5)
        return 0.0;

    float v = u_A*exp(-1.0/(k2*Lwave*Lwave))/(k2*k2);

    // take wind's direction into account
    vec2 K1 = normalize(K);

    // doesn't work on Nvidia without abs
    float a = abs(dot(K1, normalize(u_windDir)));
    float w = pow(a, u_windAlignment);
    v *= w;

    // get rid of too small waves
    w = exp(-k2*u_minWaveLength*u_minWaveLength);
    v *= w;

    return v;
}

//-------------------------------------------------------------------------------------------------
// JONSWAP: Pierson-Moscowitz + Hasselmann + dispersion

float getSpectrum(float kx, float kz)
{
    vec2 K = vec2(kx, kz);
    float k = length(K);

    if (kx == 0.0 && kz == 0.0)
        return 0.0;

    float w = sqrt(g*k);

    float alpha = 0.0081;
    float U = u_V;

    // wave frequency
    float f = w / (2 * pi);

    // peak waves frequency
    float fm = 0.13*g/U;

    // Pierson-Moscowitz
    float S = alpha*g*g/ ( pow(2*pi, 4.0)*pow(f, 5.0) ) * exp(-5.0/4.0 * pow(fm/f, 4.0));

    // Hasselmann (more peaks)
    float gamma = 3.3;
    float sigma = (f <= fm) ? 0.07 : 0.09;
    S *= exp ( log(gamma) * exp( -(f-fm)*(f-fm)/(2*sigma*sigma*fm*fm) ) );

    // wind direction dispersion
    float cosTheta = dot(normalize(u_windDir), normalize(K));
    float D = 2 / pi * cosTheta*cosTheta;
    S *= D;

    return S * 0.1;
}

//-------------------------------------------------------------------------------------------------
void main()
{
    int N = u_size;
    float L = u_physSize;

    float n = floor(gl_FragCoord.x);
    float m = floor(gl_FragCoord.y);
    n = (n < N/2) ? n : n - N;
    m = (m < N/2) ? m : m - N;

	float kx = 2*pi*n/L;
	float kz = 2*pi*m/L;

    if (u_usePhillips == 1)
    {
        // Phillips
	   float ph = getPhillips(kx, kz);

        vec2 e = texture2D(u_random, v_uv).rg;
        out_Color = vec4(e*vec2(sqrt(ph/2)), 0.0, 1.0);
        //out_Color = vec4(sqrt(ph/2), 0.0, 0.0, 1.0);
    }
    else
    {
        // Pierson-Moskowitz + Hasselmann
        float s = getSpectrum(kx, kz);
        out_Color = vec4(s, 0.0, 0.0, 1.0);
    }
}
