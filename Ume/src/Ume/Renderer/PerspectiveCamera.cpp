#include "umepch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Ume
{
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float zNear, float zFar)
	{
		m_ProjectionMatrix = glm::perspective<float>(glm::radians(fov), aspect, zNear, zFar);
		CalculateViewMatrix();
	}

	void PerspectiveCamera::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;

		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::rotate<float>(rotationMatrix, glm::radians(-rotation.x), { 1.0f, 0.0f, 0.0f });
		rotationMatrix = glm::rotate<float>(rotationMatrix, glm::radians(-rotation.y), { 0.0f, 1.0f, 0.0f });
		rotationMatrix = glm::rotate<float>(rotationMatrix, glm::radians(-rotation.z), { 0.0f, 0.0f, 1.0f });
		m_Forward = (glm::mat3)rotationMatrix * Forwad;
		m_Up = (glm::mat3)rotationMatrix * Up;

		CalculateViewMatrix();
	}

	void PerspectiveCamera::CalculateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
