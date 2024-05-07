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

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Radiance;

const float PI = 3.141592653;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 UniformSample(vec2 Xi, vec3 N)
{
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = Xi.y;
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec3 GetDir(vec2 uv)
{
    float phi = uv.x * 2.0 * PI;
    float theta = (1.0 - uv.y) * PI;
    
    float x = sin(theta) * cos(phi);
    float y = cos(theta);
    float z = sin(theta) * sin(phi);
    
    return vec3(x, y, z);
}

vec2 GetUV(vec3 dir)
{
    float phi = atan(dir.z, dir.x);
    if (phi < 0.0)
        phi += 2.0 * PI;
    float theta = acos(dir.y);

    float u = phi / (2.0 * PI);
    float v = 1.0 - theta / PI;

    return vec2(u, v);
}
    
void main()
{
    vec3 irradiance = vec3(0.0);
    int numSamples = 512; // 你可以调整采样次数以平衡性能和质量

    vec3 dir = GetDir(v_TexCoord); // 获取表面法线，假设已经存储在Radiance纹理的RGB通道中

    for (int i = 0; i < numSamples; ++i) {
        vec2 Xi = Hammersley(uint(i), uint(numSamples));
        vec3 sampleDir = UniformSample(Xi, dir);
        vec3 radiance = texture(u_Radiance, GetUV(sampleDir)).rgb;
        irradiance += radiance * max(0.0, dot(sampleDir, dir)); // 对辐照度乘以余弦值进行加权
    }

    irradiance /= float(numSamples); // 计算平均辐照度

    FragColor = vec4(irradiance, 1.0);
}
