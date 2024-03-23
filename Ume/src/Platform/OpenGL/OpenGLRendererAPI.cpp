#include "umepch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Ume
{
	void OpenGLRendererAPI::Init()
	{
		UME_PROFILE_FUNCTION();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::Clear()
	{
		UME_PROFILE_FUNCTION();
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetPolygonMode(PolygonMode mode)
	{
		GLenum glMode = GL_FILL;
		switch (mode)
		{
		case Ume::PolygonMode::Point: glMode = GL_POINT;
			break;
		case Ume::PolygonMode::Line: glMode = GL_LINE;
			break;
		case Ume::PolygonMode::Fill: glMode = GL_FILL;
			break;
		}
		
		glPolygonMode(GL_FRONT, glMode);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		UME_PROFILE_FUNCTION();

		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		UME_PROFILE_FUNCTION();

		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		UME_PROFILE_FUNCTION();

		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::EnableDepthTest(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetDepthTestFunc(DepthTestFunc func)
	{
		glDepthFunc(GL_NEVER + (int)func);
	}
}