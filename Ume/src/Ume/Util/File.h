#pragma once
#include "Ume/Core/Base.h"
#include <string>
#include "Ume/Renderer/RayTracing/Vertex.h"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	class File
	{
	public:
		static unsigned char* UmeLoadImage(const std::string& path, int& width, int& height, int& channels, bool flip);
		static std::pair<std::vector<Vertex>*, std::vector<uint32_t>*> LoadMesh(const std::string& path);
		static void SaveTexture(const Ref<Texture2D>& texture, const std::string& name);
	};
}