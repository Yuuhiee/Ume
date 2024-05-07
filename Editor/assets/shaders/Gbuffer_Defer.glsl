#type vertex
#version 430 core

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
#version 430 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

layout(location = 0) out vec4 Albedo_Roughness;
layout(location = 1) out vec4 Normal_Metalness;
layout(location = 2) out vec4 Emission;

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
uniform Light u_Lights[4];
uniform int u_LightIndex;
uniform Material u_Material;

void main()
{
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

	Albedo_Roughness = vec4(vec3(1.0, 0.0, 0.0), roughness);
	Normal_Metalness = vec4(N, metallic);
	Emission = vec4(0.0);
	
	if (u_LightIndex != -1)
	{
		Emission = vec4(u_Lights[u_LightIndex].Color * u_Lights[u_LightIndex].Intensity, 1);
	}
}