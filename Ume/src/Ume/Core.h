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
