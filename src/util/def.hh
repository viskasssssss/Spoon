#pragma once

#include <memory>

#ifdef SP_DEBUG
#if defined(SP_PLATFORM_WINDOWS)
    #define SP_DEBUGBREAK() __debugbreak()
#elif defined(SP_PLATFORM_LINUX)
    #include <signal.h>
    #define SP_DEBUGBREAK() raise(SIGTRAP)
#else
    #error "Platform doesn't support debugbreak"
#endif
#define SP_ENABLE_ASSERTS
#else
    #define SP_DEBUGBREAK()
#endif

#define SP_EXPAND_MACRO(x) x
#define SP_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define SP_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace spoon {

	template<typename T>
	using scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr scope<T> create_scope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ref<T> create_ref(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
