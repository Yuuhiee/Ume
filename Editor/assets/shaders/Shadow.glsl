#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec3 v_Position;

void main()
{
    vec4 position = u_Model * vec4(a_Position, 1.0);
    v_Position = position.xyz;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in vec3 v_Position;

void main()
{
    gl_FragDepth = gl_FragCoord.z;
}
