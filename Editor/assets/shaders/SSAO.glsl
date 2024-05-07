#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_Texcoord;
    gl_Position = vec4(a_Position.xy, 0.0, 1.0);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;

layout(location = 2) out vec4 FragBits;
layout(location = 3) out vec4 FragAlbedo;

uniform sampler2D Position;
uniform sampler2D Normal;
uniform sampler2D Albedo;
uniform sampler2D FlagBits;
uniform sampler2D u_NoiseTexture;
uniform mat4 u_ViewProjection;
uniform vec3 u_Samples[64];
uniform int u_Width;
uniform int u_Height;
uniform float radius;

const int SAMPLES = 64;

void main()
{
    vec3 flagBits = texture(FlagBits, v_TexCoord).xyz;
    int id = int(flagBits.x);
    if (id == 0) return;

    vec3 position = texture(Position, v_TexCoord).xyz;
    vec3 normal = texture(Normal, v_TexCoord).xyz;
    vec3 noise = texture(u_NoiseTexture, v_TexCoord * vec2(u_Width, u_Height) / 4.0).xyz;
    vec3 tangent = normalize(noise - normal * dot(noise, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec4 center_clip = u_ViewProjection * vec4(position, 1.0);
    float center_depth = center_clip.z / center_clip.w;

    float occlusion = 0.0;
    int sampleNum = 0;
    vec3 avgPosition = vec3(0.0);
    for (int i = 0; i < SAMPLES; ++i)
    {
        vec3 pos = position + TBN * u_Samples[i] * radius;
        
        vec4 pos_proj = u_ViewProjection * vec4(pos, 1.0);
        float depth = pos_proj.z;
        pos_proj.xyz /= pos_proj.w;
    
        vec2 uv = pos_proj.xy * 0.5 + 0.5;
        
        if (int(texture(FlagBits, uv).x) == 0) continue;

        vec3 pos_screen = texture(Albedo, uv).xyz;
        vec4 pos_screen_proj = u_ViewProjection * vec4(pos_screen, 1.0);
        float depth_screen = pos_screen_proj.z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(center_depth - depth_screen ));
        occlusion += (depth_screen <= depth - 0.001 ? 1.0 : 0.0) * rangeCheck;
        sampleNum++;
    }
    occlusion = sampleNum == 0 ? 1.0 : 1.0 - (occlusion / float(sampleNum));
    flagBits.z = occlusion;
    FragBits = vec4(flagBits, 1.0);
    //FragAlbedo = vec4(vec3(occlusion), 1.0);
}