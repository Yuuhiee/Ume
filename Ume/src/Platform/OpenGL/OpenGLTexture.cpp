#include "umepch.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "stb_image.h"
namespace Ume
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
	{
		m_Specification.Format = specification.Format;
		m_Specification.Filter = specification.Filter;
		m_Specification.Wrap = specification.Wrap;
		m_Specification.GenMips = specification.GenMips;

		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;

		GLenum format = 0, internalFormat = 0;
		switch (specification.Format)
		{
		case ImageFormat::RED8UI: internalFormat = GL_RED; format = GL_RED;  break;
		case ImageFormat::RGB: internalFormat = GL_RGB8; format = GL_RGB;	 break;
		case ImageFormat::RGBA: internalFormat = GL_RGBA8; format = GL_RGBA; break;
		default: UME_CORE_ERROR("Unsupported texture format!");
		}

		m_Format = format;
		m_InternalFormat = internalFormat;

		/*switch (channels)
		{
			case 1: internalFormat = GL_RED; format = GL_RED;  break;
			case 3: internalFormat = GL_RGB8; format = GL_RGB;	 break;
			case 4: internalFormat = GL_RGBA8; format = GL_RGBA; break;
			default: UME_CORE_ERROR("Unsupported texture format!");
		}*/

		GLenum filter = 0, wrap = 0;
		switch (specification.Filter)
		{
		case TextureFilter::Linear: filter = GL_LINEAR;  break;
		case TextureFilter::Nearest: filter = GL_NEAREST; break;
		}

		switch (specification.Wrap)
		{
		case TextureWrap::Clamp: wrap = GL_CLAMP_TO_EDGE;  break;
		case TextureWrap::Repeat: wrap = GL_REPEAT;  break;
		}
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (int)width, (int)height, 0, format, GL_UNSIGNED_BYTE, nullptr);

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
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		UME_CORE_ASSERT(data, "Failed to load image!");
		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;

		m_Specification.Format = specification.Format;
		m_Specification.Filter = specification.Filter;
		m_Specification.Wrap = specification.Wrap;
		m_Specification.GenMips = specification.GenMips;

		GLenum format = 0, internalFormat = 0;

		switch (specification.Format)
		{
		case ImageFormat::RED8UI: internalFormat = GL_RED; format = GL_RED;  break;
		case ImageFormat::RGB: internalFormat = GL_RGB8; format = GL_RGB;	 break;
		case ImageFormat::RGBA: internalFormat = GL_RGBA8; format = GL_RGBA; break;
		default: UME_CORE_ERROR("Unsupported texture format!");
		}

		m_Format = format;
		m_InternalFormat = internalFormat;

		/*switch (channels)
		{
			case 1: internalFormat = GL_RED; format = GL_RED;  break;
			case 3: internalFormat = GL_RGB8; format = GL_RGB;	 break;
			case 4: internalFormat = GL_RGBA8; format = GL_RGBA; break;
			default: UME_CORE_ERROR("Unsupported texture format!");
		}*/

		GLenum filter = 0, wrap = 0;
		switch (specification.Filter)
		{
		case TextureFilter::Linear: filter = GL_LINEAR;  break;
		case TextureFilter::Nearest: filter = GL_NEAREST; break;
		}

		switch (specification.Wrap)
		{
		case TextureWrap::Clamp: wrap = GL_CLAMP_TO_EDGE;  break;
		case TextureWrap::Repeat: wrap = GL_REPEAT;  break;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

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
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

}