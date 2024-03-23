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

		static void StoreTexture(Ref<Texture2D> texture) { s_Textures.push_back(texture); }
		static const Ref<Texture2D>& GetTexture(uint32_t index) { return s_Textures[index]; }
	private:
		static std::vector<Material*> s_Materials;
		static std::vector<Ref<Texture2D>> s_Textures;
	};
}
