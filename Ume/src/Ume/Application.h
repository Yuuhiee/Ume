#pragma once

#include "Ume/Core.h"
#include "Ume/Window.h"
#include "Ume/LayerStack.h"
#include "Ume/ImGui/ImGuiLayer.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume {

	class UME_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		bool m_Running = true;
		Scope<Window> m_Window;
		float m_LastFrameTime = 0.0f;
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

