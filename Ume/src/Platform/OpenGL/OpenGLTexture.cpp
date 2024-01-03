#include "umepch.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "stb_image.h"

Ume::OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	: m_Path(path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	UME_CORE_ASSERT(data, "Failed to load image!");
	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;
	
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*GLenum format;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

	stbi_image_free(data);
}

Ume::OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_RendererID);
}

void Ume::OpenGLTexture2D::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}
