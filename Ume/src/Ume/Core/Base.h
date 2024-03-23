#pragma once

#include <memory>

#ifdef UME_PLATFORM_WINDOWS
	#if UME_DYNAMIC_LINK
		#ifdef UME_BUILD_DLL
			#define UME_API __declspec(dllexport)
		#else
			#define UME_API __declspec(dllimport)
		#endif
	#else
		#define UME_API
	#endif
#else
	#error Ume only support Windows!
#endif

#ifdef UME_DEBUG
	#define UME_ENABLE_ASSERTS
	//#define UME_PROFILE
#endif


#ifdef UME_ENABLE_ASSERTS
	#define UME_ASSERT(x, ...) { if (!(x)) { UME_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define UME_CORE_ASSERT(x, ...) { if (!(x)) { UME_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define UME_ASSERT(x, ...)
	#define UME_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define UME_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

constexpr float PI = 3.141592653f;
constexpr float TWO_PI = 6.283185307f;
constexpr float FOUR_PI = TWO_PI * 2.0f;
constexpr float INV_PI = 0.31830988618f;
constexpr float INV_TWO_PI = 0.15915494309f;
constexpr float INV_FOUR_PI = INV_TWO_PI * 0.5f;

#define PRINT_VEC3(vec) UME_INFO("({0}, {1}, {2})", vec.x, vec.y, vec.z)

namespace Ume
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;
	template<typename T>
	Ref<T> GetRef(const WeakRef<T>& weakRef)
	{
		return weakRef.lock();
	}
}