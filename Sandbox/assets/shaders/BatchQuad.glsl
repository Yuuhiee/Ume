#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;

out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TexIndex;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = int(a_TexIndex);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_TexIndex;

out vec4 FragColor;

uniform sampler2D u_Textures[32];

void main()
{
	FragColor = texture(u_Textures[v_TexIndex], v_TexCoord) * v_Color;
}