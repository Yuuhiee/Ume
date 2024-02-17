#include "umepch.h"
#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Ume/Renderer/Renderer.h"

namespace Ume
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferDescription& description)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: UME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(description);
		}

		UME_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}
