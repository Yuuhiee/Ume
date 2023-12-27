#pragma once

#include "Core.h"

#include "Window.h"
#include "Ume/LayerStack.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume {

	class UME_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		inline void PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }
		inline void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

		bool OnWindowClose(WindowCloseEvent& e);
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

