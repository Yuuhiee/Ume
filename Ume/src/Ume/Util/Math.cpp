#include "umepch.h"
#include "Math.h"

namespace Ume
{
	bool Math::RayTriangleIntersect(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2,
		const glm::vec3& O, const glm::vec3& D,
		float& tNear, float& u, float& v)
	{
		auto E1 = P1 - P0;
		auto E2 = P2 - P0;
		auto S = O - P0;
		auto S1 = glm::cross(D, E2);
		auto S2 = glm::cross(S, E1);
		
		auto deno = 1.0f / glm::dot(S1, E1);
		float t = glm::dot(S2, E2) * deno;
		float b1 = glm::dot(S1, S) * deno;
		float b2 = glm::dot(S2, D) * deno;
		
		if (t > 0.0f && b1 > 0.0f && b2 > 0.0f && 1 - b1 - b2 > 0.0f)
		{
			tNear = t;
			u = b1;
			v = b2;
			return true;
		}
		
		return false;
	}
}