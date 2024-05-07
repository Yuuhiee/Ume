#include "umepch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "VertexArray.h"

namespace Ume
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = nullptr;
	Ref<ShaderLibrary> Renderer::s_ShaderLibrary = nullptr;
	static Ref<VertexArray> quadVA = nullptr;

	void Renderer::Init()
	{
		s_SceneData = CreateScope<SceneData>();
		s_ShaderLibrary = CreateRef<ShaderLibrary>();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::SetPolygonMode(PolygonMode mode)
	{
		RenderCommand::SetPolygonMode(mode);
	}

	void Renderer::BeginScene(const glm::mat4& viewProjection)
	{
		s_SceneData->ViewProjection = viewProjection;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::EnableDepthTest(bool enable)
	{
		RenderCommand::EnableDepthTest(enable);
	}

	void Renderer::DrawQuad()
	{
		if (!quadVA)
		{
			quadVA = VertexArray::Create(ObjectType::Quad);
		}
		Renderer::BeginScene(glm::mat4(1.0f));
		Renderer::Submit(quadVA);
		Renderer::EndScene();
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		vertexArray->GetIndexBuffer()->Bind();
		RenderCommand::DrawIndexed(vertexArray);
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