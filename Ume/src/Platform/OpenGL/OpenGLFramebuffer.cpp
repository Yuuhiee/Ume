#include "umepch.h"
#include "OpenGLFramebuffer.h"
#include "glad/glad.h"

namespace Ume
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescription& description)
		: m_Description(description)
	{
		Initialize();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLFramebuffer::Initialize()
	{
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		InitAndBindAttachments();
		Invalidate();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::InitAndBindAttachments()
	{
		auto& width = m_Description.Width;
		auto& height = m_Description.Height;
		m_DepthAttachment = Texture2D::Create(width, height, { ImageFormat::Depth });
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetRendererID(), 0);

		uint32_t count = static_cast<uint32_t>(m_Description.ColorAttachments.size());
		m_ColorAttachments.reserve(count);
		std::vector<GLuint> attachments(count);

		uint32_t i = 0;
		for (const auto& specification : m_Description.ColorAttachments)
		{
			auto texture = Texture2D::Create(width, height, specification);
			auto attachment = GL_COLOR_ATTACHMENT0 + i;
			attachments[i] = attachment;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->GetRendererID(), 0);
			m_ColorAttachments.push_back(std::move(texture));
			i++;
		}
		glDrawBuffers(count, attachments.data());
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			UME_ASSERT(false, "Framebuffer not complete!");
		}
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Description.Width, m_Description.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			m_ColorAttachments.clear();
			m_DepthAttachment = nullptr;
		}

		m_Description.Width = width;
		m_Description.Height = height;
		Initialize();
	}
}