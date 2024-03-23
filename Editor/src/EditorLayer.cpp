#include "EditorLayer.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

namespace Ume
{
	static glm::ivec2 s_SpiritPos = { 0, 0 };

	static Ref<SubTexture2D> s_SubTexture;

	static int s_PoolSize = 6000;

	static Timestep timestep = 0.0f;

	static Entity tempSquare;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		auto& window = Application::Get().GetWindow();
		window.SetVSync(false);

		m_CameraController = OrthographicCameraController((float)window.GetWidth() / (float)window.GetHeight(), 3.0f);
		m_Texture = Texture2D::Create("assets/textures/Checkerboard.png", { ImageFormat::RGB });
		m_SpiriteSheet = Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
		s_SubTexture = SubTexture2D::CreateFromTexture2D(m_SpiriteSheet, { 128, 128 }, { 7, 6 });
		//m_Texture = Texture2D::Create("assets/textures/ChernoLogo.png");

		TextureSpecification textureInfo;
		FramebufferDescription description = { window.GetWidth(), window.GetHeight() };
		description.ColorAttachments = { textureInfo, textureInfo };
		m_Framebuffer = Framebuffer::Create(description);

		m_Viewport.SetCallback(Viewport::CallbackType::Render, [&](Viewport* viewport, void* data)
		{
			ImGui::Image((void*)m_Framebuffer->GetColorAttachments()[0]->GetRendererID(), *(ImVec2*)data, { 0, 1 }, { 1, 0 });
		});
		m_Viewport.SetCallback(Viewport::CallbackType::Resize, [&](Viewport* viewport, void* data)
		{
			auto size = *(ImVec2*)data;
			m_Framebuffer->Resize(size.x, size.y);
			m_CameraController.Resize(size.x, size.y);
		});
		m_Viewport.SetCallback(Viewport::CallbackType::MouseButtonPress, [&](Viewport* viewport, void* data)
		{
			auto& info = *(Viewport::MouseClickInfo*)data;
			if (info.Buttons & MouseButtonFlagBits::Left)
			{
				float x = (float)info.Position.x;
				float y = (float)info.Position.y;
				auto width = viewport->GetSize().x;
				auto height = viewport->GetSize().y;

				auto bounds = m_CameraController.GetBounds();
				auto pos = m_CameraController.GetPosition();
				x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
				y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
				m_Particle.Position = { x + pos.x, y + pos.y };
				for (int i = 0; i < 5; i++)
					m_ParticleSystem.Emit(m_Particle);
			}
		});

		m_Scene = CreateRef<ECS>();
		m_SquareEntity = m_Scene->CreateEntity();
		m_SquareEntity.AddComponent<SpriteRenderComponent>(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		m_SquareEntity.GetComponent<Transform>().ModelMatrix = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f });

		tempSquare = m_Scene->CreateEntity();
		tempSquare.AddComponent<SpriteRenderComponent>(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		tempSquare.GetComponent<Transform>().Position = { 1.0f, 0.0f, 0.0f };
		tempSquare.GetComponent<Transform>().UpdateTransform();

		m_CameraEntity = m_Scene->CreateEntity("Entity_Camera");
		m_CameraEntity.AddComponent<CameraComponent>(m_CameraController.GetCamera().GetProjection(), true);
	}

	void EditorLayer::OnAttach()
	{
		m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
		m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
		m_Particle.SizeBegin = 0.2f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
		m_Particle.LifeTime = 1.0f;
		m_Particle.Velocity = { 0.0f, 0.0f };
		m_Particle.VelocityVariation = { 10.0f, 8.0f };
		m_Particle.Dampping = 6.0f;
		m_Particle.Position = { 0.0f, 0.0f };
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		timestep = ts;
		m_CameraController.OnUpdate(ts);
		m_CameraEntity.GetComponent<CameraComponent>().Camera = m_CameraController.GetCamera().GetProjectionMatrix();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.0f);
		RenderCommand::Clear();
		{
			//static float angle = 0.0f;
			//angle += ts * 50.0f;
			//Renderer2D::BeginScene(m_CameraController.GetCamera());
			//Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, angle, { 0.2f, 0.3f, 0.8f, 1.0f });
			//
			//for (int y = -5; y <= 5; y++)
			//{
			//	for (int x = -5; x <= 5; x++)
			//	{
			//		glm::vec3 position = { x * 0.5f, y * 0.5f, 0.05f };
			//		glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f,  (y + 5.0f) / 10.0f, 0.9f };
			//		Renderer2D::DrawQuad(position, { 0.45f, 0.45f }, 0, color);
			//	}
			//}
			//Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.06f }, { 5.0f, 5.0f }, 0.0f, m_SpiriteSheet);
			//Renderer2D::DrawQuad({ 1.0f, 1.0f, 0.07f }, { 5.0f, 5.0f }, 0.0f, s_SubTexture);
			//Renderer2D::EndScene();
			//
			//if (m_ParticleSystem.PoolSize != s_PoolSize) m_ParticleSystem.Resize(s_PoolSize);
			//m_ParticleSystem.OnUpdate(ts);
			//m_ParticleSystem.OnRender(m_CameraEntity.GetComponent<CameraComponent>().Camera.GetProjection());

			m_Scene->OnUpdate(ts);
		}
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_Viewport.IsHovered())
		{
			m_CameraController.OnEvent(e);
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool open = true;
		GUI::ShowDockSpace(&open);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		m_Viewport.Render();
		ImGui::PopStyleVar();

		Renderer2D::Stastistics stats = Renderer2D::GetStastistics();
		ImGui::Begin("Infomations");
		ImGui::Text("Time: %2fms", timestep.GetMilliseconds());
		ImGui::Text("DrawCalls: %d", stats.DrawCalls);
		ImGui::Text("QuadCount: %d", stats.QuadCount);
		ImGui::Image((void*)m_Texture->GetRendererID(), { 64, 64 });
		ImGui::End();
		Renderer2D::ResetStats();

		ImGui::Begin("Particle");
		ImGui::DragFloat2("Start Velocity", glm::value_ptr(m_Particle.Velocity), 0.2f);
		ImGui::DragFloat2("Velocity Range", glm::value_ptr(m_Particle.VelocityVariation), 0.2f);
		ImGui::DragInt("Pool Size", &s_PoolSize, 10.0f, 0);
		ImGui::Checkbox("Gravity", &m_Particle.Gravity);
		
		if (m_CameraEntity)
		{
			ImGui::Separator();

			{
				ImGui::PushID("Square");
				auto& transform = m_SquareEntity.GetComponent<Transform>();
				bool flag = false;
				flag |= GUI::Vec3Controller("Position", transform.Position);
				flag |= GUI::Vec3Controller("Rotation", transform.EulerRotation);
				flag |= GUI::Vec3Controller("Scale", transform.Scale, 1.0f);

				if (flag)
				{
					transform.UpdateTransform();
				}
				ImGui::PopID();
			}
			{
				ImGui::Separator();
				ImGui::PushID("Camera");
				auto& transform = m_CameraEntity.GetComponent<Transform>();
				bool flag = false;
				flag |= GUI::Vec3Controller("Position", transform.Position);
				flag |= GUI::Vec3Controller("Rotation", transform.EulerRotation);
				if (flag)
				{
					transform.UpdateTransform();
				}
				ImGui::PopID();
			}
		}

		////////////
		std::string label = "Position";
		static glm::vec3 values = { 1.0f, 1.0f, 1.0f };

		ImGui::End();
	}
}







//void EditorLayer::OnUpdate(Timestep ts)
//{
//	UME_PROFILE_FUNCTION();
//
//	//{
//	//	UME_PROFILE_SCOPE("CameraController::OnUpdate");
//	//	m_CameraController.OnUpdate(ts);
//	//}
//
//	{
//		UME_PROFILE_SCOPE("Render Prepare");
//		m_Framebuffer->Bind();
//		RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.0f);
//		RenderCommand::Clear();
//	}
//
//	//{
//	//	UME_PROFILE_SCOPE("Render Draw");
//	//
//	//	static float angle = 0.0f;
//	//	angle += ts * 50.0f;
//	//
//	//	Renderer2D::BeginScene(m_CameraController.GetCamera());
//	//	Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, angle, { 0.2f, 0.3f, 0.8f, 1.0f });
//	//	
//	//	for (int y = -5; y <= 5; y++)
//	//	{
//	//		for (int x = -5; x <= 5; x++)
//	//		{
//	//			glm::vec3 position = { x * 0.5f, y * 0.5f, 0.05f };
//	//			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f,  (y + 5.0f) / 10.0f, 0.9f };
//	//			Renderer2D::DrawQuad(position, { 0.45f, 0.45f }, 0, color);
//	//		}
//	//	}
//	//	Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.06f }, { 5.0f, 5.0f }, 0.0f, m_SpiriteSheet);
//	//	Renderer2D::DrawQuad({ 1.0f, 1.0f, 0.07f }, { 5.0f, 5.0f }, 0.0f, s_SubTexture);
//	//	Renderer2D::EndScene();
//	//}
//
//	m_Scene->OnUpdate(ts);
//
//	//m_ParticleSystem.OnUpdate(ts);
//	//m_ParticleSystem.OnRender(m_CameraController.GetCamera());
//	m_Framebuffer->Unbind();
//}