#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"

namespace Ume
{
	class Renderer
	{
	public:
		static void StartScene(const Ref<Camera>& camera);
		static void EndScene();
		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader> shader, const glm::mat4 modelMatrix = glm::mat4(1.0f));
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjection;
		};
		static SceneData* s_SceneData;
	};
}

