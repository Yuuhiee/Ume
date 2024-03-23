#pragma once
#include "glm/glm.hpp"

namespace Ume
{
	class CameraBase
	{
	public:
		CameraBase() = default;
		CameraBase(const glm::mat4& projection)
			: m_Projection(projection) {}

		const glm::mat4& GetProjection() const { return m_Projection; }
	private:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}