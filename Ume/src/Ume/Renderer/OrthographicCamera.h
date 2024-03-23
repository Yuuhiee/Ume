#pragma once

#include "Camera.h"

namespace Ume
{
	class OrthographicCamera : public CameraBase
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top);
		~OrthographicCamera() = default;

		void SetPosition(const glm::vec3& position) { Position = position; CalculateViewMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; CalculateViewMatrix(); }

		void CalculateViewMatrix();

		inline const glm::vec3& GetPosition() const { return Position; }
		float GetRotation() const { return m_Rotation; }
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	public:
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		void SetProjection(float left, float right, float bottom, float top);
	private:
	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};
}
