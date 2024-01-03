#pragma once

#include "Ume/Renderer/RendererAPI.h"

namespace Ume
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Clear() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}
