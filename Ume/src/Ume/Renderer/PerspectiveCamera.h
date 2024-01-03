#pragma once

#include "Camera.h"

namespace Ume
{
	const static glm::vec3 Forwad = { 0.0f, 0.0f, -1.0f };
	const static glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
	const static float Yaw = -90.0f;
	const static float Pitch = 0.0f;
	const static float Roll = 0.0f;

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspect, float zNear, float zFar);
		~PerspectiveCamera() = default;

		virtual void SetPosition(const glm::vec3& position) override { m_Position = position; CalculateViewMatrix(); }
		virtual void SetRotation(const glm::vec3& rotation) override;

		virtual inline const glm::vec3& GetPosition() const override { return m_Position; }
		virtual inline const glm::vec3& GetRotation() const override { return m_Rotation; }
		virtual inline const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }
	public:
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::vec3& GetForward() const { return m_Forward; }
		inline const glm::vec3& GetUp() const { return m_Up; }
	private:
		void CalculateViewMatrix();
	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Forward = Forwad;
		glm::vec3 m_Up = Up;
	};
}
