#pragma once

#include "Camera.h"

namespace Ume
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(float fov, float width, float height, float zNear, float zFar);
		~PerspectiveCamera() = default;

		void SetPosition(const glm::vec3& position) { m_Position = position; CalculateViewMatrix(); }
		void SetRotation(const glm::vec3& rotation);

		virtual void CalculateViewMatrix() override;

		inline const glm::vec3& GetPosition() const { return m_Position; }
		virtual inline const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }
	public:
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::vec3& GetForward() const { return m_Forward; }
		inline const glm::vec3& GetUp() const { return m_Up; }
		void SetProjection(float fov, float width, float height, float zNear, float zFar);
	private:
	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Forward = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
	};
}
