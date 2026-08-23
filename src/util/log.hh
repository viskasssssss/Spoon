#pragma once

#include "def.hh"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>


namespace spoon {
    class log {
    public:
        static void init();

        static ref<spdlog::logger>& get_core_logger() { return s_core_logger; }
        static ref<spdlog::logger>& get_client_logger() { return s_client_logger; }
    private:
        static ref<spdlog::logger> s_core_logger; // spoon logger
        static ref<spdlog::logger> s_client_logger; // plugin/client logger
    };
}


#define SP_CORE_TRACE(...)         ::spoon::log::get_core_logger()->trace(__VA_ARGS__)
#define SP_CORE_INFO(...)          ::spoon::log::get_core_logger()->info(__VA_ARGS__)
#define SP_CORE_WARN(...)          ::spoon::log::get_core_logger()->warn(__VA_ARGS__)
#define SP_CORE_ERROR(...)         ::spoon::log::get_core_logger()->error(__VA_ARGS__)
#define SP_CORE_CRITICAL(...)      ::spoon::log::get_core_logger()->critical(__VA_ARGS__)

#define SP_CLIENT_TRACE(...)    ::spoon::log::get_client_logger()->trace(__VA_ARGS__)
#define SP_CLIENT_INFO(...)     ::spoon::log::get_client_logger()->info(__VA_ARGS__)
#define SP_CLIENT_WARN(...)     ::spoon::log::get_client_logger()->warn(__VA_ARGS__)
#define SP_CLIENT_ERROR(...)    ::spoon::log::get_client_logger()->error(__VA_ARGS__)
#define SP_CLIENT_CRITICAL(...) ::spoon::log::get_client_logger()->critical(__VA_ARGS__)