#include "umepch.h"
#include "Color.h"
namespace Ume
{
	uint32_t Color::ConvertToRGBA(glm::vec4& value)
	{
		value = glm::clamp(value, glm::vec4(0.0f), glm::vec4(1.0f));
		value *= 255.0f;
		uint8_t r = (uint8_t)value.r;
		uint8_t g = (uint8_t)value.g;
		uint8_t b = (uint8_t)value.b;
		uint8_t a = (uint8_t)value.a;

		return a << 24 | b << 16 | g << 8 | r;
	}
}