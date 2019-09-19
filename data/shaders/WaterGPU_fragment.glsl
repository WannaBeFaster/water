#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec3 v_pos;
smooth in vec3 v_normal;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform vec3 u_camPos;
uniform vec3 u_camDir;
uniform samplerCube u_cubemap;

uniform vec3 u_colorOcean;
uniform vec3 u_colorLight;
uniform vec3 u_colorSky;

uniform vec3 u_lightDir;

uniform float u_Kr;
uniform float u_Kt;
uniform float u_Ks;

uniform float u_PhongFactor;

uniform int u_size;

uniform int u_renderSky;

//const vec3 lightPos = vec3(100, 500, -500);

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
vec3 hdr (vec3 color, float exposure)
{
	return 1.0 - exp(-color * exposure);
}

//-------------------------------------------------------------------------------------------------
void main()
{
	vec3 N = normalize(v_normal);

	// vector from the pos to the viewer
	vec3 V = normalize(u_camPos - v_pos);

	//
	vec3 L = normalize(u_lightDir);
	//vec3 L = normalize(lightPos - v_pos);

	// halfway
	vec3 H = normalize(L + V);

	//-------------------------------------------
	// diffuse
	float lambert = max(dot(N, L), 0.0);

	//-------------------------------------------
	// Fresnel with Schlick approximation
	float fresnel = 0.02 + 0.5*pow(1.0 - max(dot(N, V), 0.0), 5.0);

	//----------------------------------------------------------------------
	// specular (Blinn-Phong)
	float ks = pow(max(dot(H, N), 0.0), u_PhongFactor);
	vec3 specular = ks*u_colorLight;

	//----------------------------------------------------------------------
	vec3 R = reflect(-V, N);
	vec3 envColor = (u_renderSky == 1) ? texture(u_cubemap, vec3(R.x, -R.y, R.z)).rgb : u_colorSky;
	vec3 reflected = fresnel*envColor;

	// jean-phillippe like to achieve lighter waves with high positions
	vec3 transmitted = (1.0f - fresnel)*lambert*u_colorOcean*(v_pos.y + 60)/120;

	//----------------------------------------------------------------------
	vec3 color = u_Kr*reflected + u_Kt*transmitted + u_Ks*specular;

	color = hdr(color, 1.0);

	out_Color = vec4(color, 1.0);
}
