#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Attributes
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform mat4 MVP;

//-------------------------------------------------------------------------------------------------
// Out
// vertex pos (world)
smooth out vec3 v_pos;

// normal (world)
smooth out vec3 v_normal;

//-------------------------------------------------------------------------------------------------
void main()
{
	v_pos = a_pos;
	v_normal = a_normal;
	gl_Position = MVP * vec4(a_pos, 1);
}
