#pragma once

#include "glm/glm.hpp"
namespace Ume
{
	class Camera
	{
	public:
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec3& rotation) = 0;

		virtual const glm::vec3& GetPosition() const = 0;
		virtual const glm::vec3& GetRotation() const = 0;
	};
}