#include "module.hh"

#include <algorithm>

#include "log.hh"
#include "module_call_codes.hh"
#include "module_context.hh"
#include "app/window.hh"

namespace fs = std::filesystem;

static spoon::ref<spoon::module_manager> s_manager;

constexpr int INTERNAL_CALL_MAX_FORMAT_ARGS = 64;

// lua core functions implementation
extern "C" {
    int spoon_internal_call(lua_State* L) {
        std::string source = "";
        lua_Debug ar{};
        if (lua_getstack(L, 1, &ar)) {
            if (lua_getinfo(L, "Sl", &ar)) {
                source = ar.source;
            }
        } else {
            SP_CORE_WARN("Context for current script not supported, this may cause errors");
        }

        if (!source.empty() && source[0] == '@')
            source.erase(0, 1);

        std::error_code ec;

        fs::path source_path = fs::weakly_canonical(source, ec);

        if (ec) {
            return luaL_error(
                L,
                "unable to resolve Lua source: %s",
                ec.message().c_str()
            );
        }

        auto m = s_manager->get_module_by_source(source_path);

        if (!m) {
            SP_CORE_ERROR("Module not found for source: {0}", source_path.string());
            return luaL_error(L, "unable to determine current module");
        }

        // validate that the first argument is a integer (the command index)
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
        case (spoon::CALL_CODE_UI_SET_WINDOW): {
            if (args.size() != 1) {
                return luaL_error(L, "spoon._internal.call error: expected 1 argument but got %d",
                    args.size());
            }

            uint64_t id = 0;

            try {
                id = std::stoull(args[0]); 
            } 
            catch (const std::invalid_argument& e) {
                return luaL_error(L, "spoon._internal.call error: invalid argument. Expected string containing 64-bit integer (UUID) but got %s",
                    args[0].c_str());
            } 
            catch (const std::out_of_range& e) {
                return luaL_error(L, "spoon._internal.call error: the value is too large for a 64-bit integer (UUID)");
            }

            uint64_t previous = m->context.get_current_ui_window_id();
            
            m->context.set_current_ui_window_id(id);

            if (id != previous) {
                m->context.delete_callback(true);  // silently remove the previous callback if it exists
                m->context.set_callback();
            }
            break;
        }
        case (spoon::CALL_CODE_UI_START_WINDOW): {
            if (args.size() != 1) {
                return luaL_error(L, "spoon._internal.call error: expected 1 argument but got %d",
                    args.size());
            }

            m->context.add_to_pending_instructions( { spoon::UI_INSTRUCTION_START_WINDOW, {{args[0]}} } );
            m->context.set_is_started_window(true);

            return 0;
        }
        case (spoon::CALL_CODE_UI_POP): {
            if (m->context.is_started_window()) {
                m->context.add_to_pending_instructions({spoon::UI_INSTRUCTION_END_WINDOW, {}});
                
                m->context.set_is_started_window(false);
                return 0;
            }
            
            lua_warning(L, "spoon._internal.call: nothing to pop", 0); 
            break;
        }
        case (spoon::CALL_CODE_UI_BASIC_INSTRUCTION): {
            if (args.size() <= 1) {
                return luaL_error(L, "spoon._internal.call error: expected atleast 1 argument but got %d",
                    args.size());
            }

            m->context.add_to_pending_instructions(spoon::ui_instruction::create_from_arguments(args));
            break;
        }


        case (spoon::CALL_CODE_WINDOW_GET_MAIN): {
            if (spoon::window::get_all_windows().empty()) {
                return luaL_error(
                    L,
                    "spoon._internal.call error: main window doesn't exist"
                );
            }

            spoon::uuid uuid = spoon::window::get_all_windows()[0]->get_id();
            uint64_t id = static_cast<uint64_t>(uuid);

            std::string id_string = std::to_string(id);

            lua_pushlstring(
                L,
                id_string.data(),
                id_string.size()
            );

            return 1;
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

    ref<module_manager> module_manager::init(module_manager_props props) {
        s_manager = create_ref<module_manager>(props);
        s_manager->load_modules();
        return s_manager;
    }

    module_manager::module_manager(module_manager_props props) : m_props(props) {
        m_lua_state = luaL_newstate();
        luaL_openlibs(m_lua_state);

        m_event_manager = create_ref<module_event_manager>(m_lua_state);
        m_event_manager->init();

        register_spoon_namespace(m_lua_state);
    }

    module_manager::~module_manager() {
        lua_close(m_lua_state);
    }

    bool module_manager::load_modules() {
        m_pending_modules.clear();
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

        if (!m_pending_modules.empty()) {
            // run pending modules

            std::sort(m_pending_modules.begin(), m_pending_modules.end(), [](const ref<module>& a, const ref<module>& b) {
                return a->settings.priority < b->settings.priority; 
            });

            for (ref<module> m : m_pending_modules) {
                int top = lua_gettop(m_lua_state); 

                SP_CORE_TRACE("Executing module: {0}", m->settings.name);

                for (fs::path source : m->sources) {
                    if (!check_lua(luaL_dofile(m_lua_state, source.string().c_str()))) {
                        SP_CORE_ERROR("Failed to load source");
                        overall_success = false;
                    }
                }
                
                lua_settop(m_lua_state, top);
            }
        }

        return overall_success;
    }

    bool module_manager::load_module(fs::directory_entry entry) {
        SP_CORE_TRACE("Loading module: {0}", entry.path().filename().string().c_str());

        ref<module> m = create_ref<module>(this);
        m->dir = entry.path();
        //TODO:
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

            lua_pushstring(m_lua_state, "PRIORITY");
            lua_gettable(m_lua_state, -2);
            if (lua_isstring(m_lua_state, -1)) {
                m->settings.priority = lua_tointeger(m_lua_state, -1);
            }
            lua_pop(m_lua_state, 1); // PRIORITY

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

                m->sources.push_back(path);
            }
        }

        // add to pending modules
        m_pending_modules.push_back(m);

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

    void module_manager::trigger_event(const std::string& event_name, const std::string& data) {
        m_event_manager->trigger_event(event_name, data);
    }

    ref<module> module_manager::get_module_by_source(const fs::path& source)
    {
        for (auto& m : m_modules) {
            for (auto& src : m->sources) {
                if (fs::weakly_canonical(src) == source)
                    return m;
            }
        }

        return nullptr;
    }
}