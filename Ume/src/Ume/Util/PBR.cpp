#include "umepch.h"
#include "PBR.h"
#include "Ume/Renderer/Texture.h"
namespace Ume
{
    static Ref<Texture2D> EavgLut;
    static Ref<Texture2D> EmuLut;
    static glm::vec3 EdgeTint = { 0.827f, 0.792f, 0.678f };

    static glm::vec3 AverageFresnel(const glm::vec3& r, const glm::vec3& g)
    {
        return glm::vec3(0.087237f) + 0.0230685f * g - 0.0864902f * g * g + 0.0774594f * g * g * g
            + 0.782654f * r - 0.136432f * r * r + 0.278708f * r * r * r
            + 0.19744f * g * r + 0.0360605f * g * g * r - 0.2586f * g * r * r;
    }

    static glm::vec3 MultiScatterBRDF(const glm::vec3& albedo, float roughness, float NoV, float NoL)
    {
        glm::vec3 Eo = EmuLut->Sample({ NoV, roughness });
        glm::vec3 Ei = EmuLut->Sample({ NoL, roughness });
        glm::vec3 Eavg = EavgLut->Sample({ 0.0f, roughness });
        glm::vec3 Favg = AverageFresnel(albedo, EdgeTint);
        glm::vec3 Fms = (1.0f - Eo) * (1.0f - Ei) / (1.0f - Eavg) * INV_PI;
        glm::vec3 Fadd = Favg * Eavg / (1.0f - Favg * (1.0f - Eavg));
        return Fms * Fadd;
    }

    void PBR::Init()
    {
        TextureSpecification sp;
        sp.Format = ImageFormat::RGB16F;
        EavgLut = Texture2D::Create("assets/textures/lut/LUT_Eavg.png", sp);
        EmuLut = Texture2D::Create("assets/textures/lut/LUT_Emu.png", sp);
    }

    PBR::Part PBR::SeperatedBRDF(const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, bool direct)
    {
        auto F0 = glm::mix(glm::vec3(0.04f), albedo, metallic);
        auto H = glm::normalize(V + L);

        float NoV = Dot(N, V);
        float NoL = Dot(N, L);
        float NoH = Dot(N, H);
        float VoH = Dot(V, H);
        float NDF = DistributionGGX(NoH, roughness);
        float G = GeometrySmith(NoV, NoL, roughness, direct);
        auto F = FresnelSchlick(VoH, F0);

        auto nominator = NDF * G * F;
        float denominator = 4.0f * NoV * NoL + 0.0001f;
        auto specular = nominator / denominator;

        glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);

        auto whiteF0 = glm::mix(glm::vec3(0.04f), glm::vec3(1.0f), metallic);
        auto whiteKS = FresnelSchlick(VoH, whiteF0);
        auto whiteKD = (glm::vec3(1.0f) - F) * (1.0f - metallic);
        float ks = whiteKS.x / (whiteKS.x + whiteKD.x);

        return { kD * albedo * INV_PI, specular, ks };
    }

    glm::vec3 PBR::BRDF(const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, bool direct)
	{
		auto F0 = glm::mix(glm::vec3(0.04f), albedo, metallic);
		auto H = glm::normalize(V + L);

		float NoV = Dot(N, V);
		float NoL = Dot(N, L);
		float NoH = Dot(N, H);
		float VoH = Dot(V, H);
		float NDF = DistributionGGX(NoH, roughness);
		float G = GeometrySmith(NoV, NoL, roughness, direct);
		auto F = FresnelSchlick(VoH, F0);

		auto nominator = NDF * G * F;
		float denominator = 4.0f * NoV * NoL + 0.0001f;
		auto specular = nominator / denominator;

		glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);

        auto brdf = kD * albedo * INV_PI + specular;
        brdf += MultiScatterBRDF(albedo, roughness, NoV, NoL);
		return brdf;
	}

    //glm::vec3 PBR::BTDF(const glm::vec3& albedo, float roughness, float metallic, float eta, const glm::vec3& N, const glm::vec3& I, const glm::vec3& O)
    //{
    //    auto F0 = glm::mix(glm::vec3(0.04f), albedo, metallic);
    //    // refract
    //    auto H = 
    //}

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
        glm::vec3 reflection = F * (NDF * G) / (4.0f * NoV * NoL + 0.0001f); // ���乱��

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

            // ��������Ĺ���
            refraction = (1.0f - Fr) * (NDF * G) / (4.0f * NoV * NoL + 0.0001f);
        }

        // ��Ϸ�������乱��
        glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);
        glm::vec3 color = kD * albedo * INV_PI + reflection + refraction;

        return color;
	}
}