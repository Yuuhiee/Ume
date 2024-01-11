#pragma once

#include "Ume/Renderer/Texture.h"
namespace Ume
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification);
		OpenGLTexture2D(const std::string& path, const TextureSpecification& specification);
		~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(int slot = 0) const override;
		virtual bool operator==(const Texture& other) const override
		{
			return ((OpenGLTexture2D&)other).m_RendererID == m_RendererID;
		}
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		TextureSpecification m_Specification;
		int m_Format, m_InternalFormat;
	};
}

