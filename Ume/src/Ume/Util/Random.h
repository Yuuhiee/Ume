#pragma once

#include "Ume/Core/Base.h"
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Ume
{

	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static uint32_t UInt()
		{
			return s_Distribution(s_RandomEngine);
		}

		static uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static glm::vec3 Vec3()
		{
			return glm::vec3(Float(), Float(), Float());
		}

		static glm::vec3 Vec3(float min, float max)
		{
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static glm::vec3 Sphere()
		{
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}

		static glm::vec3 Hemisphere(float& pdf, float range = 1.0f)
		{
			range = std::clamp(range, 0.0f, 2.0f);
			float theta = Float() * range * PI;
			float phi = Float() * 2 * PI;
			float cosTheta = std::cos(theta);
			float sinTheta = std::sin(theta);

			glm::vec3 randomVec;
			randomVec.x = std::cos(phi) * sinTheta;
			randomVec.y = std::sin(phi) * sinTheta;
			randomVec.z = cosTheta;

			return randomVec;
		}

		static glm::vec3 TransformToNormalSpace(const glm::vec3& dir, const glm::vec3& N)
		{
			glm::vec3 T, B;
			if (glm::abs(N.x) > 0.1f)
				T = glm::normalize(glm::vec3(-N.y, N.x, 0.0f));
			else
				T = glm::normalize(glm::vec3(0.0f, -N.z, N.y));
			B = glm::normalize(glm::cross(N, T));

			glm::mat3 TBN = glm::mat3(T, B, N);
			glm::vec3 res = TBN * dir;
			return res;
		}

		static glm::vec3 Hemisphere(const glm::vec3& normal, float theta = 90.0f)
		{
			if (theta == 0.0f) return normal;
			glm::vec3 dir = Hemisphere(theta);

			return TransformToNormalSpace(dir, normal);
		}

		static glm::vec3 UniformSample(const glm::vec3& N, float& pdf);

		static glm::vec3 ImportanceSampleCos(const glm::vec3& N, float& pdf);
		
		static glm::vec3 ImportanceSampleGGX(const glm::vec3& N, const glm::vec3& V, float a, float& pdf);
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}