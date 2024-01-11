#pragma once

#include "glm/glm.hpp"
namespace Ume
{
	class Camera
	{
	public:
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		virtual void CalculateViewMatrix() = 0;
	};
}