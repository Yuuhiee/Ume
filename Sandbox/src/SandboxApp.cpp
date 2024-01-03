#include <Ume.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace Ume;

static std::string cubeVertexSrc = R"(
	#version 330 core
	layout(location = 0) in vec3 a_Position;
	layout(location = 1) in vec2 a_TexCoord;
	out vec3 v_Color;
	out vec2 v_TexCoord;
	uniform mat4 u_Model;
	uniform mat4 u_ModelViewProjection;
	void main()
	{
		gl_Position = u_ModelViewProjection * vec4(a_Position, 1);
		v_Color = 0.5 * a_Position + 0.5;
		v_TexCoord = a_TexCoord;
	}
)";

static std::string cubeFragmentSrc = R"(
	#version 330 core
	in vec3 v_Color;
	in vec2 v_TexCoord;
	out vec4 FragColor;
	uniform vec3 u_ColorTint;
	uniform sampler2D u_Texture;
	void main()
	{
		vec3 albedo = texture(u_Texture, v_TexCoord).rgb;
		FragColor = vec4(albedo * u_ColorTint, 1);
	}
)";

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

class ExampleLayer : public Layer
{
public:
	std::string Name;

	ExampleLayer(const std::string& name = "Example") : Layer(name), Name(name)
	{
		// Camera
		auto& window = Application::Get().GetWindow();
		PerspectiveCamera* camera = new PerspectiveCamera(45, (float)window.GetWidth() / window.GetHeight(), 0.1f, 500.0f);
		m_Camera.reset(camera);
		m_Camera->SetPosition(m_CameraPosition);
		m_Camera->SetRotation(m_CameraRotation);

		// Cube
		m_CubeVA = VertexArray::Create();
		m_TextureShader = Shader::Create(cubeVertexSrc, cubeFragmentSrc);
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

		m_DefaultTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_DefaultTexture->Bind();
		m_TextureShader->SetInt("u_Texture", 0);
	}

	void OnUpdate(Timestep ts) override
	{
		float moveSpeed = 1.0f;
		float rotateSpeed = 30.0f;
		UME_TRACE("Timestep | {}ms", ts.GetMilliseconds());

		if (Input::IsKeyPressed(UME_KEY_W))
			m_CameraPosition.z -= moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_S))
			m_CameraPosition.z += moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_A))
			m_CameraPosition.x -= moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_D))
			m_CameraPosition.x += moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_Q))
			m_CameraPosition.y -= moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_E))
			m_CameraPosition.y += moveSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_LEFT_SHIFT))
			m_CameraRotation.y -= rotateSpeed * ts;
		if (Input::IsKeyPressed(UME_KEY_F))
			m_CameraRotation.y += rotateSpeed * ts;

		m_Camera->SetPosition(m_CameraPosition);
		m_Camera->SetRotation(m_CameraRotation);

		RenderCommand::SetClearColor(0.1f, 0.1f, 0.11f, 1.f);
		RenderCommand::Clear();

		Renderer::StartScene(m_Camera);

		//m_Camera->SetRotation(m_Camera->GetRotation() + glm::vec3(0.0f, 0.0f, 1.0f));

		m_TextureShader->Bind();
		m_TextureShader->SetFloat3("u_ColorTint", m_CubeColorTint);

		Renderer::Submit(m_CubeVA, m_TextureShader);

		Renderer::EndScene();

	}

	void OnEvent(Ume::Event& e) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color Tint", glm::value_ptr(m_CubeColorTint));
		ImGui::End();
	}
private:
	Ref<Camera> m_Camera;
	Ref<Shader> m_TextureShader;
	Ref<VertexArray> m_CubeVA;
	Ref<Texture2D> m_DefaultTexture;

	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 3.0f };
	glm::vec3 m_CameraRotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CubeColorTint = { 1.0f, 1.0f, 1.0f };
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
