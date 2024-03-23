#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;

out vec3 Texcoords;

uniform mat4 u_ViewProjection;
uniform vec3 u_CameraPosition;

void main()
{
	Texcoords = a_Position;
    vec4 pos = u_ViewProjection * vec4(a_Position + u_CameraPosition, 1.0);
    gl_Position = pos.xyww;
}

#type fragment
#version 430 core

in vec3 Texcoords;

layout(location = 0) out vec4 FragColor;

// env texture(spherical)
uniform sampler2D u_EnvironmentMap;
uniform float u_Ambient;

const float PI = 3.14159265359;
const float INV_PI2 = 1.0 / (2.0 * PI);
const float INV_PI = 1.0 / PI;

void main()
{
    vec3 dir = normalize(Texcoords);
    float phi = acos(dir.y);
    float theta = atan(dir.z, dir.x);
    if (theta < 0.0) theta += 2.0 * PI;
    vec2 uv = vec2(theta * INV_PI2, 1.0 - phi * INV_PI);
    vec3 envColor = texture(u_EnvironmentMap, uv).rgb;
    envColor *= u_Ambient;
	FragColor = vec4(envColor, 1.0);
}