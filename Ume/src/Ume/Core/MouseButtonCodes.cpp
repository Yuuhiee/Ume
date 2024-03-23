#include "umepch.h"
#include "MouseButtonCodes.h"

namespace Ume
{

	MouseButtonFlagBits MouseButtonCodeToFlagBit(int code)
	{
		switch (code)
		{
		case UME_MOUSE_BUTTON_1: return MouseButtonFlagBits::Left;
		case UME_MOUSE_BUTTON_2: return MouseButtonFlagBits::Right;
		case UME_MOUSE_BUTTON_3: return MouseButtonFlagBits::Middel;
		}
		return MouseButtonFlagBits::Left;
	}
}