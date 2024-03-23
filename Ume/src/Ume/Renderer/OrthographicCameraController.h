#pragma once

#include "CameraController.h"
#include "OrthographicCamera.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController : public CameraController
	{
	public:
		OrthographicCameraController() = default;
		OrthographicCameraController(float aspectRatio, float zoomLevel = 1.0f, bool rotable = false);
		~OrthographicCameraController() {};

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		
		void Resize(uint32_t width, uint32_t height);
	public:
		inline const OrthographicCamera& GetCamera() const{ return m_Camera; }
		inline const OrthographicCameraBounds& GetBounds() const { return m_Bound; }
		inline const glm::vec3& GetPosition() const { return m_CameraPosition; }
	private:
		bool OnMouseScrolled(const MouseScrolledEvent& e);
		bool OnWindowResized(const WindowResizeEvent& e);
	private:
		float m_AspectRatio = 1.0f;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		OrthographicCameraBounds m_Bound;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}
