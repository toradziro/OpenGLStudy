#shader vertex
#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_textureCoord;
layout(location = 3) in float a_textureId;

uniform mat4 u_ModuleViewProjection;

out vec4 v_color;
out vec2 v_textureCoord;
out float v_textureId;

void main()
{
	gl_Position = u_ModuleViewProjection * a_position;
	v_color = a_color;
	v_textureCoord = a_textureCoord;
	v_textureId = a_textureId;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 v_color;
in vec2 v_textureCoord;
in float v_textureId;

uniform sampler2D u_Textures[2];

void main()
{
	int index = int(v_textureId);
	vec4 texColor = texture(u_Textures[index], v_textureCoord);
	color = texColor;
};