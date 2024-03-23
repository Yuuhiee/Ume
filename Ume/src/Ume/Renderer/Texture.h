#pragma once

#include "Image.h"
#include "glm/glm.hpp"

namespace Ume
{
	struct TextureSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		TextureFilter Filter = TextureFilter::Nearest;
		//TextureFilter Filter = TextureFilter::Linear;
		TextureWrap Wrap = TextureWrap::Repeat;

		bool GenMips = false;
		bool SRGB = false;
		bool Anisotropy = false;
		// uint32_t Width = 1;
		// uint32_t Height = 1;
		static void Copy(TextureSpecification& dest, const TextureSpecification& from)
		{
			dest.Format = from.Format;
			dest.Filter = from.Filter;
			dest.Wrap = from.Wrap;
			dest.GenMips = from.GenMips;
			dest.SRGB = from.SRGB;
			dest.Anisotropy = from.Anisotropy;
		}
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Bind(int slot = 0) const = 0;
		virtual std::vector<unsigned char> GetTextureData() const = 0;
		virtual const TextureSpecification& Specification() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
		virtual const std::string& GetFilePath() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual glm::vec4 Sample(const glm::vec2& texcoords) = 0;
		virtual glm::vec4 Sample(const glm::vec3& direction) = 0;
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, const TextureSpecification& specification = {});
		static Ref<Texture2D> Create(const std::string& path, const TextureSpecification& specification = {});
	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
			: m_Texture(texture)
		{
			m_Texcoords[0] = { min.x, min.y };
			m_Texcoords[1] = { max.x, min.y };
			m_Texcoords[2] = { max.x, max.y };
			m_Texcoords[3] = { min.x, max.y };
		}

		inline const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		inline const std::array<glm::vec2, 4>& GetTexcoords() const { return m_Texcoords; }

		static Ref<SubTexture2D> CreateFromTexture2D(const Ref<Texture2D>& texture, glm::vec2 spiritSize, glm::vec2 coords, glm::vec2 cellSize = { 1, 1 })
		{
			glm::vec2 min = { spiritSize.x * coords.x / texture->GetWidth(), spiritSize.y * coords.y / texture->GetHeight() };
			glm::vec2 max = { spiritSize.x * (coords.x + cellSize.x) / texture->GetWidth(), spiritSize.y * (coords.y + cellSize.y) / texture->GetHeight() };
			return CreateRef<SubTexture2D>(texture, min, max);
		}
	private:
		Ref<Texture2D> m_Texture;
		std::array<glm::vec2, 4> m_Texcoords;
	};
}