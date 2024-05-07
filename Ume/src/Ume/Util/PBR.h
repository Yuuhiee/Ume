#pragma once
#include "Ume/Core/Base.h"
#include "glm/glm.hpp"

namespace Ume
{
	class PBR
	{
	public:
		static void Init();

		static float Dot(const glm::vec3& a, const glm::vec3& b, float min = 0.0f)
		{
			return std::max(glm::dot(a, b), min);
		}
		static glm::vec3 GetF0(const glm::vec3& albedo, float metallic)
		{
			return glm::mix(glm::vec3(0.04f), albedo, metallic);
		}

		static glm::vec3 FresnelSchlick(float VoH, const glm::vec3& F0)
		{
			return F0 + (1.0f - F0) * glm::pow(glm::clamp(1.0f - VoH, 0.0f, 1.0f), 5.0f);
		}

		static glm::vec3 FresnelSchlick(float cosThetaT, float eta)
		{
			glm::vec3 r0 = ((eta - glm::vec3(1.0f)) * (eta - glm::vec3(1.0f))) / ((eta + glm::vec3(1.0f)) * (eta + glm::vec3(1.0f)));
			return r0 + (glm::vec3(1.0f) - r0) * glm::pow(1.0f - cosThetaT, 5.0f);
		}


		static float DistributionGGX(float NoH, float roughness)
		{
			float a = roughness * roughness;
			float a2 = a * a;
			float NoH2 = NoH * NoH;

			float num = a2;
			float denom = (NoH2 * (a2 - 1.0f) + 1.0f);
			denom = PI * denom * denom;

			return num / denom;
		}

		static float GeometrySchlickGGX(float NoV, float k)
		{
			float num = NoV;
			float denom = NoV * (1.0f - k) + k;

			return num / denom;
		}

		static float GeometrySmith(float NoV, float NoL, float roughness, bool direct = true)
		{
			float k = direct ? (roughness + 1.0f) * (roughness + 1.0f) * 0.125f : roughness * roughness * 0.5f;
			float ggx2 = GeometrySchlickGGX(NoV, k);
			float ggx1 = GeometrySchlickGGX(NoL, k);

			return ggx1 * ggx2;
		}

		static glm::vec3 CalculateColor(const glm::vec3& irradiance, const glm::vec3& albedo, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness, float metallic)
		{
			auto F0 = GetF0(albedo, metallic);
			auto H = glm::normalize(N + L);

			float NoV = Dot(N, V);
			float NoL = Dot(N, L);
			float NoH = Dot(N, H);
			float VoH = Dot(V, H);
			float D = DistributionGGX(NoH, roughness);
			auto F = FresnelSchlick(VoH, F0);
			float G = GeometrySmith(NoV, NoL, roughness, true);

			auto nominator = D * G * F;
			float denominator = 4.0f * NoV * NoL + 0.001f;
			auto specular = nominator / denominator;

			glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);

			return (kD * albedo * INV_PI + specular) * irradiance;
		}
		struct Part
		{
			glm::vec3 Diffuse;
			glm::vec3 Specular;
			float KS;

			operator glm::vec3() const { return Specular + Diffuse; }
		};
		static Part SeperatedBRDF(const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, bool direct = true);
		static glm::vec3 BRDF(const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, bool direct = true);
		static glm::vec3 BTDF(const glm::vec3& albedo, float roughness, float metallic, float eta, const glm::vec3& N, const glm::vec3& I, const glm::vec3& O);
		static glm::vec3 BSDF(const glm::vec3& albedo, float roughness, float metallic, float eta, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L);
	};
}
