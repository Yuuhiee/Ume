#pragma once

#include "CameraController.h"
#include "PerspectiveCamera.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume
{
	class PerspectiveCameraController : public CameraController
	{
	public:
		PerspectiveCameraController() = default;
		PerspectiveCameraController(float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f);
		~PerspectiveCameraController() {};

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		inline virtual const Camera& GetCamera() const override { return m_Camera; }
	private:
		bool OnMouseScrolled(const MouseScrolledEvent& e);
		bool OnWindowResized(const WindowResizeEvent& e);
	private:
		PerspectiveCamera m_Camera;
		
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		float m_MoveSpeed = 1.0f;
		float m_RotateSpeed = 30.0f;

		float m_FOV;
		float m_AspectRatio;
		float m_ZNear;
		float m_ZFar;
		float m_Width, m_Height;
	};
}
