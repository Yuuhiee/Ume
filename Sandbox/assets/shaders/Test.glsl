#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec3 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ModelViewProjection;

void main()
{
	gl_Position = u_ModelViewProjection * vec4(a_Position, 1);
	v_Color = 0.5 * a_Position + 0.5;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

in vec3 v_Color;
in vec2 v_TexCoord;

out vec4 FragColor;

uniform vec3 u_ColorTint;
uniform sampler2D u_Texture;
uniform sampler2D u_Texture_Logo;

void main()
{
	vec4 albedo = texture(u_Texture, v_TexCoord);
	vec4 logo = texture(u_Texture_Logo, v_TexCoord);
	vec4 color = logo * logo.a + (1.0 - logo.a) * albedo;
	FragColor = color * vec4(u_ColorTint, 1);
}