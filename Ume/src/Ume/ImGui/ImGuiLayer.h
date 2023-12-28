#pragma once

#include "Ume/Layer.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume
{
	class UME_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& e);
	private:
		float m_Time = 0.0f;
		bool m_MouseButtonPressed[5] = { false,  false,  false,  false,  false };
	private:
		bool OnKeyTyped(KeyTypedEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnKeyReleased(KeyReleasedEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	};

}
