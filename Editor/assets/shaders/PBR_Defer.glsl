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

struct Light
{
	vec3 Position;
	vec3 Color;
	float Intensity;
};

uniform sampler2D Position;
uniform sampler2D Normal;
uniform sampler2D Albedo;
uniform sampler2D MaterialParams;
uniform sampler2D Emission;
uniform sampler2D FlagBits;
uniform int u_NumLight;
uniform Light u_Lights[4];
uniform float u_EnvIntensity;
uniform sampler2D u_Radiance;
uniform sampler2D u_Irradiance;
uniform sampler2D u_BRDFLUT;
uniform vec3 u_CameraPosition;
uniform int u_Width;
uniform int u_Height;

const float PI = 3.141592653;
const float INV_PI = 0.31830988618;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

vec3 BRDF(vec3 N, vec3 V, vec3 L, vec3 albedo, float roughness, float metallic)
{
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 H = normalize(V + L);

	float NoV = max(dot(N, V), 0.0);
	float NoL = max(dot(N, L), 0.0);

    float D = DistributionGGX(N, H, roughness);   
    float G = GeometrySmith(N, V, L, roughness);      
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    //vec3 F = fresnelSchlick(NoV, F0);

	vec3 nominator = D * G * F;
	float denominator = 4.0f * NoV * NoL + 0.001f;
	vec3 specular = nominator / denominator;

	vec3 kD = (vec3(1.0f) - F) * (1.0 - metallic);

	return kD * albedo * INV_PI + specular;
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

vec3 FresnelSchlickRoughness(float VoH, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - VoH, 5.0);
}

const float weights[9] = float[](0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625);
const int kernelSize = 3;

float GetFilteredSSAO()
{
    vec2 texelSize = 1.0 / vec2(u_Width, u_Height);
    float result = 0.0;
    float totalWeight = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        for (int j = 0; j < kernelSize; ++j)
        {
            vec2 offset = vec2(i - (kernelSize / 2), j - (kernelSize / 2)) * texelSize;
            vec3 info = texture(FlagBits, v_TexCoord + offset).xyz;
            if (int(info.x) == 0) continue;
            float weight = exp(-length(offset) / (2 * 1 * 1));
            result += info.z * weight;
            totalWeight += weight;
        }
    }
    return result / totalWeight;
}

void main()
{
    vec3 flagbits = texture(FlagBits, v_TexCoord).xyz;
    int id = int(flagbits.x);
    if (id == 0) return;

    vec3 position = texture(Position, v_TexCoord).xyz;
    vec3 normal = texture(Normal, v_TexCoord).xyz;
    vec3 albedo = texture(Albedo, v_TexCoord).xyz;
    vec3 materialParams = texture(MaterialParams, v_TexCoord).xyz;
    vec3 emission = texture(Emission, v_TexCoord).xyz;
    float ssao = flagbits.z;

    vec3 N = normalize(normal);
    float roughness = materialParams.x;
    float metallic = materialParams.y;
    bool emissive = bool(flagbits.y);
    if (!emissive) emission = vec3(0.0);

	vec3 color = vec3(0.0);
	vec3 V = normalize(u_CameraPosition - position);

	for (int i = 0; i < u_NumLight; ++i)
	{
		vec3 L = normalize(u_Lights[i].Position - position);
		vec3 radiance = u_Lights[i].Color * u_Lights[i].Intensity * max(dot(N, L), 0.0);
        radiance += vec3(0.03);
		vec3 lighting = BRDF(N, V, L, albedo, roughness, metallic) * radiance;
		color += lighting;
	}

    {
		vec3 F0 = mix(vec3(0.04), albedo, metallic);
		float NoV = max(dot(N, V), 0.0);
		vec3 F = FresnelSchlickRoughness(NoV, F0, roughness);
		vec3 kd = (1.0 - F) * (1.0 - metallic);
		vec3 irradiance = texture(u_Irradiance, GetUV(N)).rgb;
		vec3 diffuse = irradiance * albedo;

		vec3 R = normalize(reflect(-V, N));
		vec3 specularIrradiance = textureLod(u_Radiance, GetUV(R), sqrt(roughness) * textureQueryLevels(u_Radiance)).rgb;
		vec2 ks = texture(u_BRDFLUT, vec2(NoV, 1.0 - roughness)).rg;
		vec3 specular = specularIrradiance * (F * ks.x + ks.y);

		color += (diffuse + specular) * u_EnvIntensity; 
	}
    color *= GetFilteredSSAO();

	FragColor = vec4(color + emission , 1);

}
