#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Attributes
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform mat4 MVP;

uniform sampler2D u_heights;
uniform sampler2D u_disp;
uniform sampler2D u_normals;

uniform int u_size;
uniform float u_worldSize;
uniform float u_dispFactor;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth out vec3 v_pos;
smooth out vec3 v_normal;

//-------------------------------------------------------------------------------------------------
// Constants

//-------------------------------------------------------------------------------------------------
// Out

//-------------------------------------------------------------------------------------------------
void main()
{
	float k = u_worldSize / u_size;

	// высота
	float h = k*texture2D(u_heights, a_uv).r;

	vec3 pos = vec3(a_pos.x, h, a_pos.z);

	// displacement vector to make waves choppy
	vec2 Dxz = k*u_dispFactor*texture2D(u_disp, a_uv).rb;

	vec3 D = vec3(Dxz.x, 0, Dxz.y);

	pos = pos - D;

	v_normal = texture2D(u_normals, a_uv).rgb;

	// don't divide by w, we need interpolated world coords
	v_pos = pos;

	gl_Position = MVP * vec4(pos, 1);
}
