#include "umepch.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "stb_image.h"
namespace Ume
{
	static glm::vec4 DefaultTextureColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

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
		case ImageFormat::RGBA16F:
			return { GL_RGBA16F, GL_RGBA, GL_FLOAT };
		case ImageFormat::DEPTH24STENCIL8:
			return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
		case ImageFormat::DEPTH32F:
			return { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT };
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

	static uint32_t GetChannel(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RED8UI:	return 1;
		case ImageFormat::RGB:		return 3;
		case ImageFormat::RGB16F:	return 3;
		case ImageFormat::RGBA:		return 4;
		case ImageFormat::RGBA16F:	return 4;
		case ImageFormat::Depth:	return 1;
		}
		UME_CORE_ERROR("Error TextureFormat!");
		return 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
	{
		TextureSpecification::Copy(m_Specification, specification);

		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;
		m_Channel = GetChannel(m_Specification.Format);

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
	static void ToSRGB(float& value)
	{
		value = value <= 0.04045f ? value / 12.92f : pow((value + 0.055f) / 1.055f, 2.4f);
	}
	static void ToSRGB(glm::vec4& color)
	{
		ToSRGB(color.r);
		ToSRGB(color.g);
		ToSRGB(color.b);
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
		m_Channel = (uint32_t)channels;

        m_Data.reserve(width * height);
        
		if (specification.StoreFormat == EStoreFormat::Vector)
		{
			if (channels == 4)
				for (int i = 0; i < width * height * channels; i += channels)
					m_Data.push_back(glm::vec4(data[i] / 255.0f, data[i + 1] / 255.0f, data[i + 2] / 255.0f, data[i + 3] / 255.0f));
			else if (channels == 3)
				for (int i = 0; i < width * height * channels; i += channels)
					m_Data.push_back(glm::vec4(data[i] / 255.0f, data[i + 1] / 255.0f, data[i + 2] / 255.0f, 1.0f));
			else if (channels == 1)
				for (int i = 0; i < width * height * channels; i += channels)
					m_Data.push_back(glm::vec4(data[i] / 255.0f, data[i] / 255.0f, data[i] / 255.0f, 1.0f));
			else
				UME_CORE_ERROR("Unsupported texture format!");
		}
		
		GLenum format = 0, internalFormat = 0, type = 0;
		if (specification.SRGB)
		{
			switch (channels)
			{
			case 3:
				internalFormat = GL_SRGB8;
				format = GL_RGB;
				break;
			case 4:
				internalFormat = GL_SRGB8_ALPHA8;
				format = GL_RGBA;
				break;
			default:
				UME_CORE_ERROR("Unsupported texture format!");
			}
			type = GL_UNSIGNED_BYTE;
			for (auto& color : m_Data) ToSRGB(color);
		}
		else
		{
			ImageFormat imageFormat = specification.Format;
			if (channels == 3) imageFormat = ImageFormat::RGB;
			if (channels == 1) imageFormat = ImageFormat::RED8UI;
			auto [i, f, t] = UmeToOpenGLTextureFormat(imageFormat);
			internalFormat = i;
			format = f;
			type = t;
		}


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
		auto info = UmeToOpenGLTextureFormat(m_Specification.Format);
		auto format = std::get<1>(info);
		auto type = std::get<2>(info);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, format, type, data);
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		if (m_RendererID)
		{
			glDeleteTextures(1, &m_RendererID);
		}
		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;

		auto [internalFormat, format, type] = UmeToOpenGLTextureFormat(m_Specification.Format);
		auto filter = UmeToOpenGLTextureFilter(m_Specification.Filter);
		auto wrap = UmeToOpenGLTextureWrap(m_Specification.Wrap);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (int)width, (int)height, 0, format, type, nullptr);

		if (m_Specification.GenMips)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	}

	void OpenGLTexture2D::Bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	glm::vec4 OpenGLTexture2D::Sample(const glm::vec2& texcoords)
	{
		float tx = texcoords.x * (m_Width - 1);
		float ty = texcoords.y * (m_Height - 1);
		uint32_t ix0 = (uint32_t)tx % m_Width;
		uint32_t iy0 = (uint32_t)ty % m_Height;

		if (m_Specification.Filter == TextureFilter::Nearest)
			return m_Data[iy0 * m_Width + ix0];

		uint32_t ix1 = ix0 + 1;
		uint32_t iy1 = iy0 + 1;

		if (ix1 == m_Width || iy1 == m_Height)
			return m_Data[iy0 * m_Width + ix0];

		float wx = tx - ix0;
		float wy = ty - iy0;

		glm::vec4 c00 = m_Data[iy0 * m_Width + ix0];
		glm::vec4 c10 = m_Data[iy0 * m_Width + ix1];
		glm::vec4 c01 = m_Data[iy1 * m_Width + ix0];
		glm::vec4 c11 = m_Data[iy1 * m_Width + ix1];

		return (1 - wx) * ((1 - wy) * c00 + wy * c01) + wx * ((1 - wy) * c10 + wy * c11);
	}

	glm::vec4 OpenGLTexture2D::Sample(const glm::vec3& direction)
	{
		float phi = std::acos(direction.y);
		float theta = std::atan2(direction.z, direction.x);
		if (theta < 0.0f)  theta += TWO_PI;

		float u = theta * INV_TWO_PI;
		float v = 1.0f - phi * INV_PI;

        return Sample({ u, v });
    }

	std::vector<unsigned char> OpenGLTexture2D::GetTextureData() const
	{
		std::vector<GLubyte> pixels(4 * m_Width * m_Height);

		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		return pixels;
	}

	OpenGLTextureCube::OpenGLTextureCube(uint32_t size, const TextureSpecification& specification)
	{
		if (m_RendererID) glDeleteTextures(1, &m_RendererID);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		auto [internalFormat, format, type] = UmeToOpenGLTextureFormat(m_Specification.Format);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size, size, 0, format, type, nullptr);

		auto filter = UmeToOpenGLTextureFilter(m_Specification.Filter);
		auto wrap = UmeToOpenGLTextureWrap(m_Specification.Wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::Bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

}