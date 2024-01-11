#pragma once

#include "CameraController.h"
#include "OrthographicCamera.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume
{
	class OrthographicCameraController : public CameraController
	{
	public:
		OrthographicCameraController() = default;
		OrthographicCameraController(float aspectRatio, bool rotable = false);
		~OrthographicCameraController() {};

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		inline virtual const Camera& GetCamera() const override { return m_Camera; }
	private:
		bool OnMouseScrolled(const MouseScrolledEvent& e);
		bool OnWindowResized(const WindowResizeEvent& e);
	private:
		float m_AspectRatio = 1.0f;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}
