#include <Ume.h>

#include "glm/glm.hpp"
#include "imgui/imgui.h"

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
	}

	void OnImGuiRender() override
	{
		/*ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();*/
	}
};

class Sandbox : public Ume::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer("Layer-1"));
	}
	~Sandbox() {}
};

Ume::Application* Ume::CreateApplication()
{
	return new Sandbox();
}
