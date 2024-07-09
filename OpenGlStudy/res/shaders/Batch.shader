#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 v_color;

out vec4 f_color;

uniform mat4 u_ModuleViewProjection;

void main()
{
	gl_Position = u_ModuleViewProjection * position;
	f_color = v_color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 f_color;

void main()
{
	color = f_color;
};