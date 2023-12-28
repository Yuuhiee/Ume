#include <Ume.h>

#include "glm/glm.hpp"

class ExampleLayer : public Ume::Layer
{
public:
	std::string Name;
	ExampleLayer(const std::string& name = "Example") : Layer(name), Name(name) {}

	void OnUpdate() override
	{
		if (Ume::Input::IsKeyPressed(UME_KEY_SPACE))
			UME_TRACE("[{}] is pressed.", (char)UME_KEY_SPACE);
	}

	void OnEvent(Ume::Event& e) override
	{
		/*Ume::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Ume::MouseButtonPressedEvent>([this](Ume::MouseButtonPressedEvent& e)-> bool
			{
				UME_TRACE("ExampleLayer::OnEvent({0}): {1}", Name, e);
				return true;
			});*/
	}
};

class Sandbox : public Ume::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer("Layer-1"));
		PushOverlay(new Ume::ImGuiLayer());
	}
	~Sandbox() {}
};

Ume::Application* Ume::CreateApplication()
{
	return new Sandbox();
}
