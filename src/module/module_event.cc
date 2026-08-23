#include "module_event.hh"

#include "log.hh"

namespace spoon {
    module_event_manager::module_event_manager(lua_State* lua_state) 
        : L(lua_state) {}

    module_event_manager::~module_event_manager() {
        for (auto& pair : event_registry) {
            for (int ref : pair.second) {
                luaL_unref(L, LUA_REGISTRYINDEX, ref);
            }
        }
    }

    void module_event_manager::init() {
        // store 'this' pointer in the registry so the static bridge can find it
        lua_pushlightuserdata(L, this);
        lua_setfield(L, LUA_REGISTRYINDEX, "__module_event_manager_instance");

        // create the global 'events' table
        lua_newtable(L);
        lua_pushcfunction(L, module_event_manager::lua_on_event_bridge); // use static bridge
        lua_setfield(L, -2, "on");
        lua_setglobal(L, "events");
    }

    void module_event_manager::trigger_event(const std::string& event_name, const std::string& data) {
        auto it = event_registry.find(event_name);
        if (it == event_registry.end()) return;

        for (int ref : it->second) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
            lua_pushstring(L, data.c_str());

            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                SP_CORE_ERROR("Lua Event Error: {0}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    }

    int module_event_manager::on_event_internal(lua_State* state) {
        if (!lua_isstring(state, 1) || !lua_isfunction(state, 2)) {
            lua_pushstring(state, "Expected: events.on(string, function)");
            lua_error(state);
            return 0;
        }

        std::string event_name = lua_tostring(state, 1);
        
        lua_pushvalue(state, 2); 
        int ref = luaL_ref(state, LUA_REGISTRYINDEX);
        
        event_registry[event_name].push_back(ref);
        return 0;
    }

    int module_event_manager::lua_on_event_bridge(lua_State* state) {
        // retrieve the cxx pointer we hid inside the Lua registry
        lua_getfield(state, LUA_REGISTRYINDEX, "__module_event_manager_instance");
        module_event_manager* instance = static_cast<module_event_manager*>(lua_touserdata(state, -1));
        lua_pop(state, 1); // clean up the stack

        if (instance) {
            return instance->on_event_internal(state);
        }
        return 0;
    }
}