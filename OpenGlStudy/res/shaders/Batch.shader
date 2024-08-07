#shader vertex
#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;

uniform mat4 u_ModuleViewProjection;

void main()
{
	gl_Position = u_ModuleViewProjection * a_position;
	v_color = a_color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 v_color;

void main()
{
	color = v_color;
};