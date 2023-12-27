#include <Ume.h>

class ExampleLayer : public Ume::Layer
{
public:
	std::string Name;
	ExampleLayer(const std::string& name = "Example") : Layer(name), Name(name) {}

	void OnUpdate() override
	{
		UME_INFO("ExampleLayer::OnUpdate({})", Name);
	}

	void OnEvent(Ume::Event& e) override
	{
		UME_TRACE("ExampleLayer::OnEvent({0}): {1}", Name, e);
	}
};

class Sandbox : public Ume::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer("Layer-1"));
		//PushLayer(new ExampleLayer("Layer-2"));
	}
	~Sandbox() {}
};

Ume::Application* Ume::CreateApplication()
{
	return new Sandbox();
}
