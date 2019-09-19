#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform samplerCube u_cubemap;
uniform vec3 u_skyColor;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec3 v_pos;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
void main()
{
	vec3 uv = v_pos;
	vec3 color = texture(u_cubemap, uv).rgb;

	out_Color = vec4(color, 1.0);
}