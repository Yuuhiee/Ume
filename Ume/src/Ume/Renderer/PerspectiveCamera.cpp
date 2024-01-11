#include "umepch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Ume
{
	const static glm::vec3 Forwad = { 0.0f, 0.0f, -1.0f };
	const static glm::vec3 Up = { 0.0f, 1.0f, 0.0f };

	PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float zNear, float zFar)
	{
		m_ProjectionMatrix = glm::perspectiveFov<float>(glm::radians(fov), width, height, zNear, zFar);
		CalculateViewMatrix();
	}

	void PerspectiveCamera::SetRotation(const glm::vec3& rotation)
	{
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
	void PerspectiveCamera::SetProjection(float fov, float width, float height, float zNear, float zFar)
	{
		if ((int)height == 0) return;
		m_ProjectionMatrix = glm::perspectiveFov<float>(glm::radians(fov), width, height, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
