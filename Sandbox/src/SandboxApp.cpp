#include <Ume.h>

class ExampleLayer : public Ume::Layer
{
public:
	std::string Name;
	ExampleLayer(const std::string& name = "Example") : Layer(name), Name(name) {}

	void OnUpdate() override
	{
		//UME_INFO("ExampleLayer::OnUpdate({})", Name);
	}

	void OnEvent(Ume::Event& e) override
	{
		Ume::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Ume::MouseButtonPressedEvent>([this](Ume::MouseButtonPressedEvent& e)-> bool
			{
				UME_TRACE("ExampleLayer::OnEvent({0}): {1}", Name, e);
				return true;
			});
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
