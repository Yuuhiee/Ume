#include "umepch.h"
#include "Random.h"
#include "PBR.h"

namespace Ume
{
	std::mt19937 Random::s_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

	glm::vec3 Random::UniformSample(const glm::vec3& N, float& pdf)
	{
		float u = Random::Float();
		float v = Random::Float();

		float cosTheta = u;
		float phi = v * TWO_PI;
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		pdf = INV_TWO_PI;
		return TransformToNormalSpace({ sinTheta * cos(phi), sinTheta * sin(phi), cosTheta }, N);
	}

	glm::vec3 Random::ImportanceSampleCos(const glm::vec3& N, float& pdf)
	{
		float u = Random::Float();
		float v = Random::Float();

		float z = sqrt(1.0f - u);
		float phi = v * TWO_PI;
		float sinTheta = sqrt(u);
		pdf = z * INV_PI;
		return TransformToNormalSpace({ sinTheta * cos(phi), sinTheta * sin(phi), z }, N);
	}

	glm::vec3 Random::ImportanceSampleGGX(const glm::vec3& N, const glm::vec3& V, float a, float& pdf)
	{
		float u = Float();
		float v = Float();
		float a2 = a * a;

		float phi = TWO_PI * u;
		float cosTheta = sqrt((1.0f - v) / (1.0f + (a2 - 1.0f) * v));
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		glm::vec3 H = { cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta };
		H = TransformToNormalSpace(H, N);

		float NoH = PBR::Dot(N, H);
		pdf = PBR::DistributionGGX(NoH, a) * NoH / (4.0f * PBR::Dot(-V, H));

		return glm::reflect(V, H);
	}
}