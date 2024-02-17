#include "umepch.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "stb_image.h"
namespace Ume
{
	static unsigned char* UmeLoadImage(const std::string& path, int& width, int& height, int& channels, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		UME_CORE_ASSERT(data, "Failed to load image!");
		return data;
	}

	static std::tuple<GLenum, GLenum, GLenum> UmeToOpenGLTextureFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RED8UI:
			return { GL_RED, GL_RED, GL_UNSIGNED_BYTE };
		case ImageFormat::RGB:
			return { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };
		case ImageFormat::RGB16F:
			return { GL_RGB16F, GL_RGB, GL_FLOAT };
		case ImageFormat::RGBA:
			return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
		case ImageFormat::Depth:
			return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
		}
		UME_CORE_ERROR("Error TextureFormat!");
		return { 0, 0, 0 };
	}

	static std::pair<GLenum, GLenum> OpenGLTextureFormatFromChannel(int channel)
	{
		switch (channel)
		{
		case 1:
			return { GL_RED, GL_RED };
		case 3:
			return { GL_RGB8, GL_RGB };
		case 4:
			return { GL_RGBA8, GL_RGBA };
		}
		UME_CORE_ERROR("Error TextureFormat!");
		return { 0, 0 };
	}

	static GLenum UmeToOpenGLTextureFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case Ume::TextureFilter::Nearest:
			return GL_NEAREST;
		case Ume::TextureFilter::Linear:
			return GL_LINEAR;
		case Ume::TextureFilter::Mipmap:
			return GL_LINEAR_MIPMAP_LINEAR;
		}
		UME_CORE_ERROR("Error TextureFilter!");
		return 0;
	}

	static GLenum UmeToOpenGLTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Clamp:
			return GL_CLAMP_TO_EDGE;
		case TextureWrap::Repeat:
			return GL_REPEAT;
		}
		UME_CORE_ERROR("Error TextureWrap!");
		return 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
	{
		TextureSpecification::Copy(m_Specification, specification);

		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;

		auto [internalFormat, format, type] = UmeToOpenGLTextureFormat(specification.Format);

		/*switch (channels)
		{
			case 1: internalFormat = GL_RED; format = GL_RED;  break;
			case 3: internalFormat = GL_RGB8; format = GL_RGB;	 break;
			case 4: internalFormat = GL_RGBA8; format = GL_RGBA; break;
			default: UME_CORE_ERROR("Unsupported texture format!");
		}*/

		auto filter = UmeToOpenGLTextureFilter(specification.Filter);
		auto wrap = UmeToOpenGLTextureWrap(specification.Wrap);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (int)width, (int)height, 0, format, type, nullptr);

		if (specification.GenMips)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSpecification& specification)
		: m_Path(path)
	{
		TextureSpecification::Copy(m_Specification, specification);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		UME_CORE_ASSERT(data, "Failed to load image!");
		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;

		auto [internalFormat, format, type] = UmeToOpenGLTextureFormat(specification.Format);

		auto filter = UmeToOpenGLTextureFilter(specification.Filter);
		auto wrap = UmeToOpenGLTextureWrap(specification.Wrap);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

		if (specification.GenMips)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		auto format = std::get<1>(UmeToOpenGLTextureFormat(m_Specification.Format));
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

}