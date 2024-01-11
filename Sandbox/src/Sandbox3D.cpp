#include "Sandbox3D.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace Ume;

static float cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.5f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.5f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,

		 -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		  0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		  0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

		 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		  0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		  0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f
};

static uint32_t cubeIndices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

Sandbox3D::Sandbox3D()
	: Layer("Sandbox3D")
{// Camera
	auto& window = Application::Get().GetWindow();
	window.SetVSync(false);

	m_PerspectiveCameraController = PerspectiveCameraController(45, (float)window.GetWidth(), (float)window.GetHeight(), 0.1f, 500.0f);

	// Cube
	m_CubeVA = VertexArray::Create();
	Renderer::GetShaderLibrary()->Load("assets/shaders/Texture.glsl");

	{
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		vertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			});
		Ref<IndexBuffer> indexBuffer(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
		m_CubeVA->AddVertexBuffer(vertexBuffer);
		m_CubeVA->SetIndexBuffer(indexBuffer);
	}
	m_DefaultTexture = Texture2D::Create("assets/textures/Checkerboard.png", { ImageFormat::RGB });
	m_LogoTexture = Texture2D::Create("assets/textures/ChernoLogo.png", { ImageFormat::RGB });
}

void Sandbox3D::OnUpdate(Timestep ts)
{
	m_PerspectiveCameraController.OnUpdate(ts);

	RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.f);
	RenderCommand::Clear();

	Renderer::StartScene(m_PerspectiveCameraController.GetCamera());

	//m_Camera->SetRotation(m_Camera->GetRotation() + glm::vec3(0.0f, 0.0f, 1.0f));

	const auto& textureShader = Renderer::GetShaderLibrary()->Get("Texture");

	textureShader->Bind();
	textureShader->SetFloat3("u_ColorTint", m_CubeColorTint);

	m_DefaultTexture->Bind();
	textureShader->SetInt("u_Texture", 0);

	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	for (int y = -1; y <= 1; y++)
	{
		position.y = y * 1.2f;
		for (int x = -1; x <= 1; x++)
		{
			position.x = x * 1.2f;
			Renderer::Submit(m_CubeVA, textureShader, glm::translate(glm::mat4(1.0f), position));
		}
	}

	Renderer::EndScene();
}

void Sandbox3D::OnEvent(Event& e)
{
	m_PerspectiveCameraController.OnEvent(e);
}

void Sandbox3D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Color Tint", glm::value_ptr(m_CubeColorTint));
	ImGui::End();
}
