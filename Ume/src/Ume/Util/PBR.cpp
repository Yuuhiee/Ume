#include "umepch.h"
#include "PBR.h"

namespace Ume
{
	glm::vec3 PBR::BRDF(const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L)
	{
		auto F0 = glm::mix(glm::vec3(0.04f), albedo, metallic);
		auto H = glm::normalize(V + L);

		float NoV = Dot(N, V);
		float NoL = Dot(N, L);
		float NoH = Dot(N, H);
		float VoH = Dot(V, H);
		float NDF = DistributionGGX(NoH, roughness);
		float G = GeometrySmith(NoV, NoL, roughness);
		auto F = FresnelSchlick(VoH, F0);

		auto nominator = NDF * G * F;
		float denominator = 4.0f * NoV * NoL + 0.0001f;
		auto specular = nominator / denominator;

		glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);

		return kD * albedo * INV_PI + specular;
	}

	glm::vec3 PBR::BSDF(const glm::vec3& albedo, float roughness, float metallic, float eta, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L)
	{
        auto F0 = glm::mix(glm::vec3(0.04f), albedo, metallic);
        auto H = glm::normalize(V + L);

        float NoV = Dot(N, V);
        float NoL = Dot(N, L);
        float NoH = Dot(N, H);
        float VoH = Dot(V, H);
        float NDF = DistributionGGX(NoH, roughness);
        float G = GeometrySmith(NoV, NoL, roughness);
        auto F = FresnelSchlick(VoH, F0);

        glm::vec3 refractDir = glm::refract(-V, N, eta);

        float cosThetaT = Dot(-V, N);
        float sinThetaT2 = 1.0f - eta * eta * (1.0f - cosThetaT * cosThetaT);

        bool totalInternalReflection = sinThetaT2 < 0.0f;
        glm::vec3 reflection = F * (NDF * G) / (4.0f * NoV * NoL + 0.0001f); // ·´Éä¹±Ï×

        float refraction = 0.0f;
        if (!totalInternalReflection)
        {
            auto F1Vec = FresnelSchlick(cosThetaT, eta);
            auto F2Vec = FresnelSchlick(cosThetaT, 1.0f / eta);
            float F1 = (F1Vec.r + F1Vec.g + F1Vec.b) / 3.0f;
            float F2 = (F2Vec.r + F2Vec.g + F2Vec.b) / 3.0f;
            float Rs = (F1 + F2) * 0.5f;
            float Rp = (F1 * F1 + F2 * F2) * 0.5f;
            float Fr = 0.5f * (Rs + Rp);
            float Ft = 1.0f - Fr;

            // ¼ÆËãÕÛÉäµÄ¹±Ï×
            refraction = (1.0f - Fr) * (NDF * G) / (4.0f * NoV * NoL + 0.0001f);
        }

        // ×éºÏ·´ÉäºÍÕÛÉä¹±Ï×
        glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);
        glm::vec3 color = kD * albedo * INV_PI + reflection + refraction;

        return color;
	}
}