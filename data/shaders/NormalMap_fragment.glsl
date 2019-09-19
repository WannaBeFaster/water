#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform sampler2D u_slope;
uniform int u_size;
uniform float u_smoothNormals;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
vec3 calcNormalFromSlope(vec2 S)
{
	//S *= -1;

	float slen = length(S);
	vec3 N = (vec3(0.0, 1.0, 0.0) - vec3(S.x, 0.0, S.y)) / sqrt(1.0 + slen*slen);

	return N;
}

//-------------------------------------------------------------------------------------------------
void main()
{
	vec2 uv = gl_FragCoord.xy / u_size;
	float texel = 1.0 / u_size;

	vec3 N = calcNormalFromSlope( texture2D(u_slope, uv).rb );

	vec3 Nu = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x, uv.y + texel)).rb );
	vec3 Nb = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x, uv.y - texel)).rb );
	vec3 Nl = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x - texel, uv.y)).rb );
	vec3 Nr = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x + texel, uv.y)).rb );

	vec3 Nul = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x - texel, uv.y + texel)).rb );
	vec3 Nur = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x + texel, uv.y + texel)).rb );
	vec3 Nbl = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x - texel, uv.y - texel)).rb );
	vec3 Nbr = calcNormalFromSlope( texture2D(u_slope, vec2(uv.x + texel, uv.y - texel)).rb );

	N = normalize(N + u_smoothNormals * ( 0.25*(Nu + Nb + Nl + Nr) + 0.08*(Nul + Nur + Nbl + Nbr)));

	out_Color = vec4(N, 1.0);
}
