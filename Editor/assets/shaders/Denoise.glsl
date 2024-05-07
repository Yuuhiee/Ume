#type vertex
#version 430 core

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
#version 430 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_PreviousImage;
uniform sampler2D u_CurrentImage;
uniform sampler2D u_PositionImage;
uniform sampler2D u_NormalImage;
uniform sampler2D u_IDImage;
uniform int Width; 
uniform int Height;
uniform int Filter;
uniform int Accumulate;
uniform float Frame;
uniform float s_Pos;
uniform float s_Nor;
uniform float s_Col;
uniform float s_Pla;

float length2(vec3 v)
{
    return dot(v, v);
}

float safeAcos(float x)
{
    return acos(clamp(x, 0.0, 1.0));
}

const float sigmaPosition = 1.0 / (2.0 * pow(32.0, 2.0));
const float sigmaNormal   = 1.0 / (2.0 * pow( 0.1, 2.0));
const float sigmaColor    = 1.0 / (2.0 * pow( 0.6, 2.0));
const float sigmaPlane    = 1.0 / (2.0 * pow( 0.1, 2.0));


const bool wavelet = true;
const int kernelSize = 16;
const int passes = 6;

vec3 previous;
vec3 position;
vec3 normal;
vec3 current;
int ID;
vec3 filtered = vec3(0.0);
float totalWeight = 0.0;
vec2 pStep;

float norm2(vec3 v)
{
    float l = length(v);
    return l * l;
}

void PerSample(vec2 offset)
{
    vec2 texcoord = v_TexCoord + offset * pStep;
    int id = int(texture(u_IDImage, texcoord).r);
    if (id != ID || id == 0) return;

    vec3 pos = texture(u_PositionImage, texcoord).rgb;
    vec3 nor = texture(u_NormalImage, texcoord).rgb;
    vec3 color = texture(u_CurrentImage, texcoord).rgb;
        
    //float d_pos = length2(pos - position) * sigmaPosition;
    //float d_nor = pow(safeAcos(dot(nor, normal)), 1.0) * sigmaNormal;
    //float d_color = length2(color - current) * sigmaColor;
    //float d_plane = d_pos > 0.0 ? pow(dot(nor, normalize(pos - position)), 2.0) * sigmaPlane : 0.0;
    float d_pos = norm2(pos - position) / (2.0 * pow(s_Pos, 2.0));
    float d_nor = pow(safeAcos(dot(nor, normal)), 2.0) / (2.0 * pow(s_Nor, 2.0));
    float d_color = norm2(color - current) / (2.0 * pow(s_Col, 2.0));
    float d_plane = d_pos > 0.0 ? pow(dot(nor, normalize(pos - position)), 2.0) / (2.0 * pow(s_Pla, 2.0)) : 0.0;

    float weight = exp(-d_pos - d_nor - d_color - d_plane);
    filtered += color * weight;
    totalWeight += weight;
}

void main()
{
    previous = texture(u_PreviousImage, v_TexCoord).rgb;
    position = texture(u_PositionImage, v_TexCoord).rgb;
    normal = normalize(texture(u_NormalImage, v_TexCoord).xyz);
    current = texture(u_CurrentImage, v_TexCoord).rgb;
    ID = int(texture(u_IDImage, v_TexCoord).r);

    if (bool(Filter) && ID != 0)
    {
        pStep = 1.0 / vec2(Width, Height);
        if (wavelet)
        {
            for (int pass = 0; pass < passes; pass++)
                for (int i = -2; i <= 2; i++)
                    for (int j = -2; j <= 2; j++)
                        PerSample(vec2(i, j) * pass * pass);
        }
        else
        {
            for (int i = -kernelSize; i <= kernelSize; i++)
                for (int j = -kernelSize; j <= kernelSize; j++)
                    PerSample(vec2(i, j));
        }
        filtered /= totalWeight;
        current = filtered;
    }
    current = clamp(current, vec3(0.0), vec3(10.0));
    //current = max(current, vec3(0.0));
    vec3 color = bool(Accumulate) ? (previous * (Frame - 1.0) + current) / Frame : current;
    FragColor = vec4(color, 1.0);
}