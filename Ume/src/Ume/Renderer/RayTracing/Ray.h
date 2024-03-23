#pragma once

#include "glm/glm.hpp"

namespace Ume
{
	struct Ray
	{
		glm::vec3 Origin;
		glm::vec3 Direction;
		glm::vec3 DirectionInv;

		Ray() = default;
		Ray(const glm::vec3& ori, const glm::vec3& dir)
			: Origin(ori), Direction(dir), DirectionInv(1.0f / dir) {}
	};
}