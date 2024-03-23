#pragma once

#include "Ume/Core/Flags.h"

namespace Ume
{
	enum class SampleMethodFlagBits : UmeFlags
	{
		ImportanceCos = 1 << 0,
		ImportanceGGX = 1 << 1,
		ImportanceLight = 1 << 2,
	};
	using SampleMethodFlags = Flags<SampleMethodFlagBits>;
}