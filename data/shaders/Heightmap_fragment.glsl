#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform sampler2D u_tx;
uniform int u_size;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec2 v_uv;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
void main()
{
	float h = 2*texture2D(u_tx, v_uv).r;
	out_Color = vec4(h, h, h, 1.0);
}
