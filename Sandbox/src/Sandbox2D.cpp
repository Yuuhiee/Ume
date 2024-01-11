#include "Sandbox2D.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace Ume;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
{
	auto& window = Application::Get().GetWindow();
	window.SetVSync(false);

	m_CameraController = OrthographicCameraController((float)window.GetWidth() / (float)window.GetHeight());
	m_Texture = Texture2D::Create("assets/textures/Checkerboard.png", { ImageFormat::RGB });

}

void Sandbox2D::OnUpdate(Timestep ts)
{
	UME_PROFILE_FUNCTION();

	//UME_TRACE(1000 / (int)ts.GetMilliseconds());

	{
		UME_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	{
		UME_PROFILE_SCOPE("Render Prepare");
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.f);
		RenderCommand::Clear();
	}

	{
		UME_PROFILE_SCOPE("Render Draw");

		static float angle = 0.0f;
		angle += ts * 50.0f;

		Renderer2D::StartScene(m_CameraController.GetCamera());
		Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 5.0f, 5.0f }, 0.0f, m_Texture);
		Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, angle, { 0.2f, 0.3f, 0.8f, 1.0f });

		for (int y = -5; y <= 5; y++)
		{
			for (int x = -5; x <= 5; x++)
			{
				glm::vec3 position = { x * 0.5f, y * 0.5f, -0.05f };
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f,  (y + 5.0f) / 10.0f, 0.9f };
				Renderer2D::DrawQuad(position, { 0.45f, 0.45f }, 0, color);
			}
		}

		Renderer2D::EndScene();
	}
}

void Sandbox2D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Infomations");

	Renderer2D::Stastistics stats = Renderer2D::GetStastistics();
	ImGui::Text("DrawCalls: %d", stats.DrawCalls);
	ImGui::Text("QuadCount: %d", stats.QuadCount);
	ImGui::End();
	Renderer2D::ResetStats();
}
