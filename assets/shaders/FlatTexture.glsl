#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec3 v_Color;
out vec3 v_Position;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ModelViewProjection;

void main()
{
	gl_Position = u_ModelViewProjection * vec4(a_Position, 1);

	v_TexCoord = a_TexCoord;
	v_Color = 0.5 * a_Position + 0.5;
	v_Position = mat3(u_Model) * a_Position;
}

#type fragment
#version 330 core

in vec3 v_Color;
in vec2 v_TexCoord;
in vec3 v_Position;

out vec4 FragColor;

uniform vec3 u_ColorTint;
uniform sampler2D u_Texture;
// uniform sampler2D u_Texture_Logo;

void main()
{
	vec4 albedo = texture(u_Texture, v_TexCoord);
	vec4 color;
	if (v_Position.z > 0)
		color = albedo * 0.5;
	else
		color = albedo;
	FragColor = color * vec4(u_ColorTint, 1.0);
	// FragColor = vec4(v_Position, 1.0);
}