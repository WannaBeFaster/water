#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform sampler2D u_h0;
uniform sampler2D u_random;
uniform float u_time;
uniform float u_physSize;
uniform int u_size;

//-------------------------------------------------------------------------------------------------
// Constants
const float g = 9.81;
const float pi = 3.14159265358;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
float tanh(float x)
{
	float e = exp(2*x);
	return (e-1)/(e+1);
}

//-------------------------------------------------------------------------------------------------
vec2 multiplyComplex(vec2 a, vec2 b)
{
	return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

//-------------------------------------------------------------------------------------------------
// Animate waves height spectrum

void main()
{
	float N = u_size;
	float L = u_physSize;

	float n = floor(gl_FragCoord.x);
	float m = floor(gl_FragCoord.y);
	vec2 uv = vec2(n, m) / N;

	n = (n < N/2) ? n : n - N;
	m = (m < N/2) ? m : m - N;

	vec2 K = vec2(2*pi*n/L, 2*pi*m/L);
	float k = length(K);

	vec2 h0;
	vec2 h0star;
	float wt;

	if (k == 0.0)
	{
		h0 = vec2(0);
		h0star = vec2(0);
		wt = 0.0;
	}
	else
	{
		float wk = 0.0;

		// true for deep ocean
		wk = sqrt(g*k);

		// quanterize frequency (T = 5 mins)
		float w0 = 2*pi/300.0;
		float w = w0*floor(wk/w0);

		float phi0 = texture2D(u_random, uv).b;
		wt = mod(w*u_time + phi0, 2*pi);

		// Tessendorf (43)
		h0 = texture2D(u_h0, uv).rg;
		vec2 uv2 = vec2(1.0) - uv - 1.0/u_size;
		h0star = texture2D(u_h0, uv2).rg;
		h0star.y *= -1;
	}

	vec2 cosin = vec2(cos(wt), sin(wt));
	vec2 res = multiplyComplex(h0, cosin) + multiplyComplex(h0star, vec2(cosin.x, -cosin.y));

	out_Color = vec4(res.x, res.y, 0.0, 0.0);
}
