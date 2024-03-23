#pragma once

#include "Ume/Core/Flags.h"

#define UME_MOUSE_BUTTON_1         0
#define UME_MOUSE_BUTTON_2         1
#define UME_MOUSE_BUTTON_3         2
#define UME_MOUSE_BUTTON_4         3
#define UME_MOUSE_BUTTON_5         4
#define UME_MOUSE_BUTTON_6         5
#define UME_MOUSE_BUTTON_7         6
#define UME_MOUSE_BUTTON_8         7
#define UME_MOUSE_BUTTON_LAST      UME_MOUSE_BUTTON_8
#define UME_MOUSE_BUTTON_LEFT      UME_MOUSE_BUTTON_1
#define UME_MOUSE_BUTTON_RIGHT     UME_MOUSE_BUTTON_2
#define UME_MOUSE_BUTTON_MIDDLE    UME_MOUSE_BUTTON_3

namespace Ume
{
	enum class MouseButtonFlagBits : UmeFlags
	{
		Left = 1 << 0,
		Right = 1 << 1,
		Middel = 1 << 2,
	};
	using MouseButtonFlags = Flags<MouseButtonFlagBits>;

	MouseButtonFlagBits MouseButtonCodeToFlagBit(int code);

	enum class CursorMode
	{
		Normal = 0,
		Hidden = 1,
		Locked = 2,
	};
}