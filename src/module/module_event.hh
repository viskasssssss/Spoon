#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>
extern "C"
{
    #include <lua550/lua.h>
    #include <lua550/lauxlib.h>
    #include <lua550/lualib.h>
}

#include "def.hh"

namespace spoon {
    class module_event_manager {
    public:
        module_event_manager(lua_State* lua_state);
        ~module_event_manager();

        void init();
        void trigger_event(const std::string& event_name, const std::string& data);
    private:
        std::unordered_map<std::string, std::vector<int>> event_registry;
        lua_State* L;

        int on_event_internal(lua_State* state);
        static int lua_on_event_bridge(lua_State* state);
    };
}