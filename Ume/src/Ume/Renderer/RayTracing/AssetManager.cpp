#include "umepch.h"
#include "AssetManager.h"

namespace Ume
{
	std::vector<Material*>		AssetManager::s_Materials;
	std::vector<Ref<Texture2D>> AssetManager::s_Textures;

	void AssetManager::Init()
	{
		auto defaultMaterial = new Material();
		s_Materials = { defaultMaterial };

		auto defaultTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		s_Textures = { defaultTexture };
	}
}