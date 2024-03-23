#pragma once
#include "Ume/Core/Base.h"
#include "glm/glm.hpp"

namespace Ume
{
	class Color
	{
	public:
		static uint32_t ConvertToRGBA(glm::vec4& value);
	};
}