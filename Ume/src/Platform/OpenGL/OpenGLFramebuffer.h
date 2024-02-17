#pragma once
#include "Ume/Renderer/Framebuffer.h"

namespace Ume
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferDescription& description);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		inline virtual uint32_t GetRendererID() const override { return m_RendererID; }
		inline virtual const FramebufferDescription& GetDescription() const override { return m_Description; }
		virtual const Ref<Texture2D>& GetDepthAttachment() const override { return m_DepthAttachment; }
		virtual const std::vector<Ref<Texture2D>>& GetColorAttachments() const override { return m_ColorAttachments; }
	private:
		uint32_t m_RendererID = 0;
		Ref<Texture2D> m_DepthAttachment;
		std::vector<Ref<Texture2D>> m_ColorAttachments;
		FramebufferDescription m_Description;

		void Initialize();
		void InitAndBindAttachments();
		void Invalidate();
	};
}

