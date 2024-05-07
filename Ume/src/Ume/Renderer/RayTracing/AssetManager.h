#pragma once
#include "Material.h"

namespace Ume
{
	class AssetManager
	{
	public:
		static void Init();
		static Material& StoreMaterial(Material* material)
		{
			material->ID = (int)s_Materials.size();
			s_Materials.push_back(material);
			return *material;
		}
		static Material& GetMaterial(int index) { return  *s_Materials[index]; }
		static const Ref<Texture2D>& DefaultTexture() { return s_Textures["assets/textures/Checkerboard.png"]; }
		static const Ref<Texture2D>& LoadTexture(const std::string& path, const TextureSpecification& sp = {});
		static void PostRender();
	private:
		static std::vector<Material*> s_Materials;
		static std::unordered_map<std::string, Ref<Texture2D>> s_Textures;
	};
}
