#pragma once

#include "glm/glm.hpp"

namespace Ume
{
	class Math
	{
	public:
		static bool RayTriangleIntersect(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& O, const glm::vec3& D, float& tNear, float& u, float& v);

		template<typename T>
		static T TriangleLerp(const T& v0, const T& v1, const T& v2, float u, float v)
		{
			return (1 - u - v) * v0 + u * v1 + v * v2;
		}
	};
}