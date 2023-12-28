#pragma once

#ifdef UME_PLATFORM_WINDOWS
	#ifdef UME_BUILD_DLL
		#define UME_API _declspec(dllexport)
	#else
		#define UME_API _declspec(dllimport)
	#endif // UME_BUILD_DLL
#else
	#error Ume only support Windows!
#endif // UME_PLATFORM_WINDOWS

#ifdef UME_ENABLE_ASSERTS
	#define UME_ASSERT(x, ...) { if (!x) { UME_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define UME_CORE_ASSERT(x, ...) { if (!x) { UME_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define UME_ASSERT(x, ...)
	#define UME_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define UME_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)