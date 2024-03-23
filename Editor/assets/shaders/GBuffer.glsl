#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_Position;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat3 u_NormalMatrix;

void main()
{
	vec4 position = u_Model * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjection * position;

	v_Position = position.xyz;
	v_Normal = u_NormalMatrix * a_Normal;
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec3 v_Normal;

layout(location = 0) out vec4 FragPosition;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragID;

uniform int u_ObjectID;

void main()
{
	FragPosition = vec4(v_Position, 1.0);
	FragNormal = vec4(normalize(v_Normal), 1.0);
	FragID = vec4(u_ObjectID, 0, 0, 1);
}