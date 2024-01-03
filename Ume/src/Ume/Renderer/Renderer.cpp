#include "umepch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Ume
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::StartScene(const Ref<Camera>& camera)
	{
		s_SceneData->ViewProjection = camera->GetViewProjectionMatrix();
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
}