#pragma once

#include "glm/glm.hpp"
#include "Ume/Core/Timestep.h"
#include "Ume/Core/Base.h"

namespace Ume
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float FoV, uint32_t width, uint32_t height, float zNear, float zFar);
		virtual ~Camera() = default;

		virtual bool Resize(uint32_t width, uint32_t height);
		virtual void Recalculate();

		inline const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
		inline const glm::mat4& GetView() const { return m_View; }
		inline const glm::mat4& GetInverseView() const { return m_InverseView; }
		inline const uint32_t GetWidth() const { return m_ViewportWidth; }
		inline const uint32_t GetHeight() const { return m_ViewportHeight; }
		inline const glm::vec2& GetPixelSize() const { return m_PixelSize; }

		float Sensitivity = 0.6f;
		float MoveSpeed = 5.0f;

		float FoV = 45.0f;
		float Pitch = 0.0f;
		float Yaw = -180.0f;
		glm::vec3 Forawrd{ 0.0f, 0.0f, 1.0f };
		glm::vec3 Right{ 1.0f, 0.0f, 0.0f };
		glm::vec3 Up{ 0.0f, 1.0f, 0.0f };
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
	protected:
		void RecalculateProjection();
		void RecalculateView();
	protected:
		glm::mat4 m_ViewProjection{ 1.0f };
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };
		glm::mat4 m_InverseProjection{ 1.0f };
		glm::mat4 m_InverseView{ 1.0f };

		float m_NearClip = 0.1f;
		float m_FarClip = 100.0f;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		glm::vec2 m_PixelSize = { 0.0f, 0.0f };
	};

	class RTCamera : public Camera
	{
	public:
		RTCamera() = default;
		RTCamera(float FoV, uint32_t width, uint32_t height, float zNear, float zFar)
			: Camera({ FoV, width, height, zNear, zFar }) {
			RecalculateRayDirections();
		}
		virtual ~RTCamera() = default;
		virtual bool Resize(uint32_t width, uint32_t height) override
		{
			bool resized = Camera::Resize(width, height);
			if (resized)
			{
				RecalculateRayDirections();
			}
			return resized;
		}
		virtual void Recalculate() override
		{
			Camera::Recalculate();
			RecalculateRayDirections();
		}
		inline const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }
		inline const glm::vec3& GetRayDirection(int index) const { return m_RayDirections[index]; }
		inline const glm::vec3& GetRayDirection(int x, int y) const { return m_RayDirections[y * m_ViewportWidth + x]; }
	private:
		std::vector<glm::vec3> m_RayDirections;
		void RecalculateRayDirections();
	};

	class EditorCameraController
	{
	public:
		EditorCameraController() = default;
		EditorCameraController(const Ref<Camera>& camera) : m_Camera(camera) {}
		const Ref<Camera>& GetCamera() const { return m_Camera; }
		void Update(Timestep ts);
	private:
		Ref<Camera> m_Camera;
	};
}
