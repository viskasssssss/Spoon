#pragma once

#include "def.hh"
#include "log.hh"

#include <filesystem>

#ifdef SP_ENABLE_ASSERTS

    #define SP_INTERNAL_ASSERT_IMPL(log, check, msg, ...) \
        do { \
            if (!(check)) { \
                log(msg, __VA_ARGS__); \
                SP_DEBUGBREAK(); \
            } \
        } while (false)

    #define SP_INTERNAL_ASSERT_WITH_MSG(log, check, ...) \
        SP_INTERNAL_ASSERT_IMPL(log, check, "Assertion failed: {0}", __VA_ARGS__)

    #define SP_INTERNAL_ASSERT_NO_MSG(log, check) \
        SP_INTERNAL_ASSERT_IMPL( \
            log, \
            check, \
            "Assertion '{0}' failed at {1}:{2}", \
            SP_STRINGIFY_MACRO(check), \
            std::filesystem::path(__FILE__).filename().string(), \
            __LINE__ \
        )

    #define SP_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

    #define SP_INTERNAL_ASSERT_GET_MACRO(...) \
        SP_EXPAND_MACRO( \
            SP_INTERNAL_ASSERT_GET_MACRO_NAME( \
                __VA_ARGS__, \
                SP_INTERNAL_ASSERT_WITH_MSG, \
                SP_INTERNAL_ASSERT_NO_MSG \
            ) \
        )

    #define SP_ASSERT(...) \
        SP_EXPAND_MACRO( \
            SP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)( \
                SP_CLIENT_ERROR, \
                __VA_ARGS__ \
            ) \
        )

    #define SP_CORE_ASSERT(...) \
        SP_EXPAND_MACRO( \
            SP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)( \
                SP_CORE_ERROR, \
                __VA_ARGS__ \
            ) \
        )

#else

    #define SP_ASSERT(...)
    #define SP_CORE_ASSERT(...)

#endif