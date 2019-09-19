#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform vec3 u_camPos;
uniform vec3 u_camDir;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec3 v_pos;
smooth in vec3 v_normal;

//-------------------------------------------------------------------------------------------------
// Constants
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);
const vec3 diffuseColor = vec3(0.0, 1.0, 1.0);

// vector to the light
const vec3 L = normalize(vec3(100, 100, -100));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

const float phongFactor = 512.0;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
void main()
{
	vec3 color = vec3(0, 0, 0);
	vec3 N = normalize(v_normal);

	//----------------------------------------------------------------------
	// ambient
	color = color + ambientColor;

	//----------------------------------------------------------------------
	// diffuse
	float lambert = dot(N, L);
	color = color + diffuseColor*max(lambert, 0.0);

	//----------------------------------------------------------------------
	// specular (Blinn-Phong)
	/*

	// вектор от освещаемой точки к камере
	vec3 V = normalize(u_camPos - v_pos);

	// "медиана"
	vec3 H = normalize(L+V);
	float ks = pow(max(dot(H, N), 0.0), phongFactor);
	color = color + lightColor*ks;
	*/

	out_Color = vec4(color, 1.0);
}
