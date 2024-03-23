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

void main()
{
	vec4 position = u_Model * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjection * u_Model * position;

	v_Position = vec3(position);
	v_Normal = a_Normal;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;
uniform vec3 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;

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
	vec3 H = normalize(N + L);

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
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_CameraPosition - v_Position);
	vec3 L = normalize(u_LightPosition - v_Position);

	vec3 irradiance = vec3(1.0) * max(dot(N, L), 0.0);
	vec3 lighting = BRDF(N, V, L, u_Albedo, u_Roughness, u_Metallic) * irradiance;

	vec3 R = normalize(reflect(-V, N));
	vec3 env = R * 0.5 + 0.5; // 暂时用出射方向代替

	vec3 color = vec3(0.0);
	color += lighting;
	//color += env * BRDF(N, V, R, u_Albedo, u_Roughness, u_Metallic) * max(dot(N, R), 0.0);
	//color = env * BRDF(N, V, R, u_Albedo, u_Roughness, u_Metallic) * max(dot(N, R), 0.0);
	//vec3 F0 = mix(vec3(0.04), u_Albedo, u_Metallic); 
    //vec3 F = fresnelSchlick(clamp(dot(N, V), 0.0, 1.0), F0);
	//color = F;

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
	FragColor = vec4(color, 1);
}