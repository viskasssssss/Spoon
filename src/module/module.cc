#include "module.hh"

#include "log.hh"
#include "module_call_codes.hh"

namespace fs = std::filesystem;

constexpr int INTERNAL_CALL_MAX_FORMAT_ARGS = 64;

// lua core functions implementation
extern "C" {
    int spoon_internal_call(lua_State* L) {
        // validate that the first argument is a string (the format template)
        if (!lua_isinteger(L, 1)) {
            luaL_error(L, "bad argument #1 to '_internal.call' (int expected)");
            return 0;
        }
        int code = lua_tointeger(L, 1);
        
        // count arguments and enforce the 64-argument ceiling
        int total_stack_elements = lua_gettop(L);
        int value_args_count = total_stack_elements - 1; // exclude the format string itself
        
        if (value_args_count > INTERNAL_CALL_MAX_FORMAT_ARGS) {
            return luaL_error(L, "spoon._internal.call error: exceeded maximum limit of %d formatting arguments (provided %d)", 
                INTERNAL_CALL_MAX_FORMAT_ARGS, value_args_count);
        }
        
        // fetch subsequent arguments from the stack and store them as strings
        std::vector<std::string> args;
        args.reserve(value_args_count);
        
        for (int i = 2; i <= total_stack_elements; i++) {
            size_t len;
            const char* str = luaL_tolstring(L, i, &len);
            args.push_back(std::string(str, len));
            lua_pop(L, 1); // clean up the temporary string created by luaL_tolstring
        }
        
        switch (code) {
            case (spoon::CALL_CODE_PRINT): {
                // 1 argument = message
            if (args.size() != 1) {
                return luaL_error(L, "spoon._internal.call error: expected 1 argument but got %d",
                    args.size());
            }
            SP_CLIENT_INFO(args[0].c_str());
            break;
        }
        default: {
            SP_CLIENT_WARN("Unknown call code: {0}", code);
            break;
        }
        }

        return 0;
    }
}


namespace spoon {

    static const struct luaL_Reg spoon_functions[] = {
        {NULL, NULL} // array terminator
    };

    static const struct luaL_Reg spoon_internal_functions[] = {
        {"call", spoon_internal_call},
        {NULL, NULL}
    };

    // helper function for registering namespaces and functions
    void register_spoon_namespace(lua_State* L) {
        lua_newtable(L);
        
        luaL_setfuncs(L, spoon_functions, 0); 
        int spoon_table_index = lua_gettop(L);

        lua_newtable(L); 
        
        luaL_setfuncs(L, spoon_internal_functions, 0);

        lua_setfield(L, spoon_table_index, "_internal"); 
        
        lua_setglobal(L, "spoon");
    }

    module_manager::module_manager(module_manager_props props) : m_props(props) {
        m_lua_state = luaL_newstate();
        luaL_openlibs(m_lua_state);

        event_manager = create_ref<module_event_manager>(m_lua_state);
        event_manager->init();

        register_spoon_namespace(m_lua_state);

        load_modules();
    }

    module_manager::~module_manager() {
        lua_close(m_lua_state);
    }

    bool module_manager::load_modules() {
        // scan for modules
        bool overall_success = true;
        try {
            if (fs::exists(m_props.modules_path) && fs::is_directory(m_props.modules_path)) {
                // loop through each entry in the directory
                for (const auto& entry : fs::directory_iterator(m_props.modules_path)) {
                    // check if the current entry is a directory
                    if (fs::is_directory(entry.status())) {
                        if (!load_module(entry)) {
                            SP_CORE_ERROR("Failed to load module");
                            overall_success = false;
                        }
                    }
                }
            } else {
                SP_CORE_CRITICAL("The module folder ""{0}"" does not exist", m_props.modules_path.c_str());
                return false;
            }
        } catch (const fs::filesystem_error& err) {
            SP_CORE_CRITICAL("Got filesystem error: {0} while iterating over modules", err.what());
            return false;
        }
        return overall_success;
    }

    bool module_manager::load_module(fs::directory_entry entry) {
        SP_CORE_TRACE("Loading module: {0}", entry.path().filename().string().c_str());

        ref<module> m = create_ref<module>();
        m_modules.push_back(m);

        bool has_module_file = false;
        fs::path module_file_path = "";

        for (const auto& child_entry : fs::directory_iterator(entry.path())) {
                            
            if (fs::is_regular_file(child_entry.status()) && 
                (child_entry.path().filename() == "module.lua" || 
                child_entry.path().filename() == "MODULE.lua")) {
                has_module_file = true;
                module_file_path = child_entry.path();
                break;
            }
        }

        if (!has_module_file) {
            SP_CORE_ERROR(
                "Can not load module with directory name ""{0}"". The module's directory does not have the module settings file",
                    entry.path().filename().string().c_str()
            );
            return false;
        }

        int top = lua_gettop(m_lua_state); 

        if (!check_lua(luaL_dofile(m_lua_state, module_file_path.string().c_str()))) { lua_settop(m_lua_state, top); return false; }

        // get the settings from the module settings file

        if (lua_istable(m_lua_state, -1)) {
            
            lua_pushstring(m_lua_state, "ID");
            lua_gettable(m_lua_state, -2); // Looks inside the table at index -2 (the returned config table)
            if (lua_isstring(m_lua_state, -1)) {
                m->settings.id = lua_tostring(m_lua_state, -1);
            }
            lua_pop(m_lua_state, 1); // ID

            lua_pushstring(m_lua_state, "NAME");
            lua_gettable(m_lua_state, -2);
            if (lua_isstring(m_lua_state, -1)) {
                m->settings.name = lua_tostring(m_lua_state, -1);
            }
            lua_pop(m_lua_state, 1); // NAME

            lua_pushstring(m_lua_state, "VERSION");
            lua_gettable(m_lua_state, -2);
            if (lua_isstring(m_lua_state, -1)) {
                m->settings.version = lua_tostring(m_lua_state, -1);
            }
            lua_pop(m_lua_state, 1); // VERSION

            lua_pushstring(m_lua_state, "SPOON_API");
            lua_gettable(m_lua_state, -2);
            if (lua_isstring(m_lua_state, -1)) {
                m->settings.spoon_api = lua_tostring(m_lua_state, -1);
            }
            lua_pop(m_lua_state, 1); // SPOON_API

        } else {
            SP_CORE_ERROR("Module configuration file \"{0}\" did not return a valid configuration table", module_file_path.filename().string().c_str());
            lua_settop(m_lua_state, top);
            return false;
        }

        lua_settop(m_lua_state, top);

        for (const auto& child_entry : fs::recursive_directory_iterator(entry.path())) {
                            
            if (fs::is_regular_file(child_entry.status()) && child_entry.path().extension() == ".lua") {
                std::string filename = child_entry.path().filename().string();
                fs::path path = child_entry.path();
                if (path == module_file_path) continue;

                try {
                    if (filename.at(0) == '_' || filename.at(0) == '~') {
                        continue;
                    }
                } catch (const std::out_of_range& e) {}
                
                // try to run this file

                if (!check_lua(luaL_dofile(m_lua_state, path.string().c_str()))) {
                    // failed to run this filee
                    lua_settop(m_lua_state, top);
                    return false;
                }
            }
        }

        return true;
    }

    bool module_manager::check_lua(int r) {
        if (r != LUA_OK) {
            SP_CORE_ERROR(lua_tostring(m_lua_state, -1));
            lua_pop(m_lua_state, 1);
            return false;
        }
        return true;
    }
}