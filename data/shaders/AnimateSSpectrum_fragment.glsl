#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform float u_physSize;
uniform int u_size;
uniform sampler2D u_hkt;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec2 v_uv;

//-------------------------------------------------------------------------------------------------
// Constants
const float pi = 3.14159265358;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
vec2 multiplyComplex(vec2 a, vec2 b)
{
	return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

//-------------------------------------------------------------------------------------------------
// Animate slope spectrum

void main()
{
	float N = u_size;
	float L = u_physSize;

    float n = floor(gl_FragCoord.x);
    float m = floor(gl_FragCoord.y);
    n = (n < N/2) ? n : n - N;
	m = (m < N/2) ? m : m - N;

	vec2 K = vec2(2*pi*n/L, 2*pi*m/L);

	vec2 hkt = texture2D(u_hkt, v_uv).rg;

	// Tessendorf (37)
	vec2 slopeX = K.x*multiplyComplex(vec2(0.0, 1.0), hkt);
	vec2 slopeZ = K.y*multiplyComplex(vec2(0.0, 1.0), hkt);

	// rg - slope x
	// ba - slope z
	out_Color = vec4(slopeX.x, slopeX.y, slopeZ.x, slopeZ.y);
}
