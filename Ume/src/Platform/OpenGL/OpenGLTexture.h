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
		inline virtual uint32_t GetRendererID() const override { return m_RendererID; }
		inline virtual const std::string& GetFilePath() const override { return m_Path; }

		virtual void SetData(void* data, uint32_t size) override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Bind(int slot = 0) const override;
		virtual glm::vec4 Sample(const glm::vec2& texcoords) override;
		virtual glm::vec4 Sample(const glm::vec3& direction) override;
		virtual std::vector<unsigned char> GetTextureData() const override;
		virtual const TextureSpecification& Specification() const override { return m_Specification; }

		virtual bool operator==(const Texture& other) const override
		{
			return ((OpenGLTexture2D&)other).m_RendererID == m_RendererID;
		}
	private:
		std::vector<glm::vec4> m_Data;
		std::string m_Path;
		uint32_t m_Width, m_Height, m_Channel = 3;
		uint32_t m_RendererID;
		TextureSpecification m_Specification;
	};
}

