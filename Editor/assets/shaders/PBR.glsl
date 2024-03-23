#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat3 u_NormalMatrix;

void main()
{
	vec4 position = u_Model * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjection * position;

	v_Position = position.xyz;
	v_Normal = u_NormalMatrix * a_Normal;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

layout(location = 0) out vec4 FragColor;

struct Light
{
	vec3 Position;
	vec3 Color;
	float Intensity;
};

struct Material
{
	vec3 ColorTint;

	vec3 Albedo;
	int UseAlbedoTexture;
	sampler2D AlbedoTexture;

	float Roughness;
	int UseRoughnessTexture;
	sampler2D RoughnessTexture;

	float Metallic;
	int UseMetallicTexture;
	sampler2D MetallicTexture;

	int UseNormalTexture;
	sampler2D NormalTexture;
};

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;
uniform Light u_Lights[4];
uniform int u_NumLight;
uniform int u_LightIndex;
uniform float u_ObjectID;
uniform Material u_Material;

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

void main()
{
	if (u_LightIndex != -1)
	{
		FragColor = vec4(u_Lights[u_LightIndex].Color, 1);
		return;
	}
	vec3 color = vec3(0.0);
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_CameraPosition - v_Position);

    if (bool(u_Material.UseNormalTexture))
    {
        vec3 normal = texture(u_Material.NormalTexture, v_TexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        vec3 Nt = normalize(N);
        vec3 Nb = normalize(cross(N, V));
        mat3 TBN = mat3(Nb, Nt, N);
        N = normalize(TBN * normal);
    }

	vec3 albedo = bool(u_Material.UseAlbedoTexture) ? texture(u_Material.AlbedoTexture, v_TexCoord).rgb : u_Material.Albedo;
	albedo *= u_Material.ColorTint;

	float roughness = bool(u_Material.UseRoughnessTexture) ? texture(u_Material.RoughnessTexture, v_TexCoord).r : u_Material.Roughness;
	float metallic = bool(u_Material.UseMetallicTexture) ? texture(u_Material.MetallicTexture, v_TexCoord).r : u_Material.Metallic;

	for (int i = 0; i < u_NumLight; ++i)
	{
		vec3 L = normalize(u_Lights[i].Position - v_Position);
		vec3 radiance = u_Lights[i].Color * u_Lights[i].Intensity * max(dot(N, L), 0.0);
		vec3 lighting = BRDF(N, V, L, albedo, roughness, metallic) * radiance;
		color += lighting;
	}

	FragColor = vec4(color, 1);
}