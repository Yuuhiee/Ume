#include "Sandbox2D.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace Ume;

static glm::ivec2 s_SpiritPos = { 0, 0 };

static Ref<SubTexture2D> s_SubTexture;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
{
	auto& window = Application::Get().GetWindow();
	window.SetVSync(false);

	m_CameraController = OrthographicCameraController((float)window.GetWidth() / (float)window.GetHeight());
	m_Texture = Texture2D::Create("assets/textures/Checkerboard.png", { ImageFormat::RGB });
	m_SpiriteSheet = Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
	s_SubTexture = SubTexture2D::CreateFromTexture2D(m_SpiriteSheet, { 128, 128 }, { 7, 6 });
	//m_Texture = Texture2D::Create("assets/textures/ChernoLogo.png");

	FramebufferDescription description;
	description.Width = window.GetWidth();
	description.Height = window.GetHeight();
	description.ColorAttachments = { {} };
	m_Framebuffer = Framebuffer::Create(description);
}

void Sandbox2D::OnAttach()
{
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.2f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 10.0f, 8.0f };
	m_Particle.Dampping = 0.03f;
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnUpdate(Timestep ts)
{
	UME_PROFILE_FUNCTION();

	{
		UME_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	{
		UME_PROFILE_SCOPE("Render Prepare");
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.0f);
		RenderCommand::Clear();
	}

	{
		UME_PROFILE_SCOPE("Render Draw");

		static float angle = 0.0f;
		angle += ts * 50.0f;

		Renderer2D::StartScene(m_CameraController.GetCamera());
		//Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.02f }, { 5.0f, 5.0f }, 0.0f, m_Texture);
		Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, angle, { 0.2f, 0.3f, 0.8f, 1.0f });

		for (int y = -5; y <= 5; y++)
		{
			for (int x = -5; x <= 5; x++)
			{
				glm::vec3 position = { x * 0.5f, y * 0.5f, 0.05f };
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f,  (y + 5.0f) / 10.0f, 0.9f };
				Renderer2D::DrawQuad(position, { 0.45f, 0.45f }, 0, color);
			}
		}
		Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.06f }, { 5.0f, 5.0f }, 0.0f, m_SpiriteSheet);
		Renderer2D::DrawQuad({ 1.0f, 1.0f, 0.07f }, { 5.0f, 5.0f }, 0.0f, s_SubTexture);
		//Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.06f }, { 5.0f, 5.0f }, 0.0f, m_SpiriteSheet);
		Renderer2D::EndScene();

		if (Input::IsMouseButtonPressed(UME_MOUSE_BUTTON_1))
		{
			auto [x, y] = Input::GetMousePosition();
			auto& window = Application::Get().GetWindow();
			auto width = window.GetWidth();
			auto height = window.GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());

		m_Framebuffer->Unbind();
	}
}

void Sandbox2D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent& e)
		{
			switch (e.GetKeyCode())
			{
			case UME_KEY_RIGHT: s_SpiritPos.x++;
			case UME_KEY_UP: s_SpiritPos.y++;
			}
			return false;
		});
}

static void ShowDockSpace(bool* p_open)
{
	static bool opt_fullscreen_persistant = true;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	bool opt_fullscreen = opt_fullscreen_persistant;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Dockspace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void Sandbox2D::OnImGuiRender()
{
	static bool open = true;
	ShowDockSpace(&open);

	ImGui::Begin("Window");
	auto viewportSize = ImGui::GetContentRegionAvail();
	m_Framebuffer->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	m_CameraController.Resize(viewportSize.x / viewportSize.y);
	ImGui::Image((void*)m_Framebuffer->GetColorAttachments()[0]->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
	ImGui::End();

	ImGui::Begin("Infomations");
	Renderer2D::Stastistics stats = Renderer2D::GetStastistics();
	ImGui::Text("DrawCalls: %d", stats.DrawCalls);
	ImGui::Text("QuadCount: %d", stats.QuadCount);
	ImGui::Image((void*)m_Texture->GetRendererID(), { 64, 64 });

	ImGui::End();
	Renderer2D::ResetStats();

	ImGui::Begin("Particle");
	ImGui::DragFloat2("Start Velocity", glm::value_ptr(m_Particle.Velocity), 0.2f);
	ImGui::DragFloat2("Velocity Range", glm::value_ptr(m_Particle.VelocityVariation), 0.2f);
	ImGui::Checkbox("Gravity", &m_Particle.Gravity);
	ImGui::End();
}
