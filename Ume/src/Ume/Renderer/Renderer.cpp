#include "umepch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"

namespace Ume
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = nullptr;
	Ref<ShaderLibrary> Renderer::s_ShaderLibrary = nullptr;

	void Renderer::Init()
	{
		s_SceneData = CreateScope<SceneData>();
		s_ShaderLibrary = CreateRef<ShaderLibrary>();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::StartScene(const Camera& camera)
	{
		s_SceneData->ViewProjection = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader> shader, const glm::mat4 modelMatrix)
	{
		shader->Bind();
		shader->SetMat4("u_Model", modelMatrix);
		shader->SetMat4("u_ModelViewProjection", s_SceneData->ViewProjection * modelMatrix);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}