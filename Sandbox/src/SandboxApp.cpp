#include <Ume.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace Ume;

static std::string cubeVertexSrc = R"(
	#version 330 core
	layout(location = 0) in vec3 a_Position;
	out vec3 v_Color;
	uniform mat4 u_Model;
	uniform mat4 u_ModelViewProjection;
	void main()
	{
		gl_Position = u_ModelViewProjection * vec4(a_Position, 1);
		v_Color = 0.5 * a_Position + 0.5;
	}
)";

static std::string cubeFragmentSrc = R"(
	#version 330 core
	in vec3 v_Color;
	out vec4 FragColor;
	uniform vec3 u_ColorTint;
	void main()
	{
		FragColor = vec4(v_Color * u_ColorTint, 1);
	}
)";

static float cubeVertices[] = {
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f
};

static uint32_t cubeIndices[] = {
	0, 1, 2, 2, 3, 0,
	3, 2, 6, 6, 7, 3,
	7, 6, 5, 5, 4, 7,
	4, 5, 1, 1, 0, 4,
	4, 0, 3, 3, 7, 4,
	1, 5, 6, 6, 2, 1,
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
		m_CubeShader = Shader::Create(cubeVertexSrc, cubeFragmentSrc);
		{
			Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
			vertexBuffer->SetLayout({ {ShaderDataType::Float3, "a_Position"} });
			Ref<IndexBuffer> indexBuffer(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
			m_CubeVA->AddVertexBuffer(vertexBuffer);
			m_CubeVA->SetIndexBuffer(indexBuffer);
		}
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

		m_CubeShader->Bind();
		m_CubeShader->SetFloat3("u_ColorTint", m_CubeColorTint);

		Renderer::Submit(m_CubeVA, m_CubeShader);

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
	Ref<Shader> m_CubeShader;
	Ref<VertexArray> m_CubeVA;

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
