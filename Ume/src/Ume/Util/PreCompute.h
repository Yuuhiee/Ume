#pragma once

#include "Ume/Core/Base.h"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	class PreCompute
	{
	public:
		static Ref<Texture2D> GetIrradiance(const Ref<Texture2D>& radiance);
	};
}