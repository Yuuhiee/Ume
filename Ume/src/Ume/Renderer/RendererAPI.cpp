#include "umepch.h"
#include "RendererAPI.h"

#include "Ume/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Ume
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case API::None:
			UME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		}

		UME_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}