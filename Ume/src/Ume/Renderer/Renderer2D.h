#pragma once

#include "Camera.h"
#include "Texture.h"

#include "glm/glm.hpp"

namespace Ume
{
	class Renderer2D
	{
	public:
		static void Init();
		static void StartScene(const Camera& camera);
		static void EndScene();
		static void Flush();
		static void FlushAndReset();
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);

		struct Stastistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static Stastistics GetStastistics();
		static void ResetStats();
	private:
	};
}
