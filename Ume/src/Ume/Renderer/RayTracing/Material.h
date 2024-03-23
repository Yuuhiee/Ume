#pragma once
#include "Ume/Core/Base.h"
#include "glm/glm.hpp"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	enum class LightType : uint8_t { Point, Directional };
	static std::vector<const char*> LightTypeNames = { "Point", "Directional" };

	struct Emission
	{
		glm::vec3 Color{ 1.0f };
		float Intensity = 1.0f;
		LightType Type = LightType::Point;
		float Range = 20.0f;
		glm::vec3 Attenuation{ 1.0f, 0.0f, 0.0f };
		glm::vec3 Direction{ 0.0f, -1.0f, 0.0f };
	};

	struct Material
	{
		uint32_t ID = 0;
		glm::vec3 ColorTint{ 1.0f };
		glm::vec3 Albedo = { 0.6f, 0.6f, 0.6f };
		float Roughness = 0.5f;
		float Metallic = 0.0f;
		bool Emissive = false;
		Emission* Emission = nullptr;

		bool UseAlbedoTexture = false;
		bool UseNormalTexture = false;
		bool UseRoughnessTexture = false;
		bool UseMetallicTexture = false;

		Ref<Texture2D> AlbedoTexture = nullptr;
		Ref<Texture2D> NormalTexture = nullptr;
		Ref<Texture2D> RoughnessTexture = nullptr;
		Ref<Texture2D> MetallicTexture = nullptr;
	};
}