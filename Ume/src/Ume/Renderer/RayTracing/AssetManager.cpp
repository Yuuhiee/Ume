#include "umepch.h"
#include "AssetManager.h"

namespace Ume
{
	std::vector<Material*>		AssetManager::s_Materials;
	std::unordered_map<std::string, Ref<Texture2D>> AssetManager::s_Textures;

	void AssetManager::Init()
	{
		auto defaultMaterial = new Material();
		s_Materials = { defaultMaterial };

		auto defaultTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		s_Textures = { { "assets/textures/Checkerboard.png", defaultTexture } };
	}

	const Ref<Texture2D>& Ume::AssetManager::LoadTexture(const std::string& path, const TextureSpecification& sp)
	{
		if (s_Textures.count(path) && s_Textures[path]) return s_Textures[path];

		auto texture = Texture2D::Create(path, sp);
		s_Textures[path] = texture;
		return texture;
	}

	void AssetManager::PostRender()
	{
		for (auto& p : s_Textures)
		{
			if (p.second.use_count() == 1)
			{
				p.second = nullptr;
			}
		}
	}
}