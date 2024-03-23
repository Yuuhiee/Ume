#pragma once
#include "Texture.h"

namespace Ume
{
	struct FramebufferDescription
	{
		uint32_t Width, Height;
		std::vector<TextureSpecification> ColorAttachments;
		uint32_t Samples = 1;
		bool SwapchainTarget = false;
	};
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual const FramebufferDescription& GetDescription() const = 0;
		virtual const std::vector<Ref<Texture2D>>& GetColorAttachments() const = 0;
		virtual const Ref<Texture2D>& GetDepthAttachment() const = 0;

		static Ref<Framebuffer> Create(const FramebufferDescription& description);
	};
}