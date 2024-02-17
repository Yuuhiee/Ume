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
		static void Init();
		static void StartScene(const Camera& camera);
		static void EndScene();
		static void EnableDepthTest(bool enable);
		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader> shader, const glm::mat4 modelMatrix = glm::mat4(1.0f));
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		inline static const Ref<ShaderLibrary>& GetShaderLibrary() { return s_ShaderLibrary; }
	public:
		static void OnWindowResize(uint32_t width, uint32_t height);
	private:
		struct SceneData
		{
			glm::mat4 ViewProjection;
		};
		static Scope<SceneData> s_SceneData;
		static Ref<ShaderLibrary> s_ShaderLibrary;
	};
}

