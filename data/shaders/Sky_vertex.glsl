#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Attributes
layout(location = 0) in vec3 a_position;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform mat4 MVP;

//-------------------------------------------------------------------------------------------------
// Out
smooth out vec3 v_pos;

//-------------------------------------------------------------------------------------------------
void main()
{
	v_pos = vec3(a_position.x, -a_position.y, a_position.z);
	gl_Position = MVP * vec4(a_position, 1);	
}