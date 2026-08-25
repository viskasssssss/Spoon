#pragma once


#include <filesystem>
#include <vector>
#include <string>
#include <stdexcept>
extern "C"
{
    #include <lua550/lua.h>
    #include <lua550/lauxlib.h>
    #include <lua550/lualib.h>
}

#include "def.hh"
#include "module_event.hh"
#include "module_context.hh"


namespace spoon {
    struct module_manager_props {
        std::string modules_path = "./modules/";
    };

    struct module_settings {
        std::string id = "NONE";
        std::string name = "NONE";
        std::string version = "0.0.0";
        std::string spoon_api = "0.0";
        int priority = 0;
    };

    struct module {
        std::filesystem::path dir;
        std::vector<std::filesystem::path> sources;
        module_settings settings;
        module_context context;

        module(module_manager* manager)
        : context(manager)
        {
        }
    };


    class module_manager {
    public:
        static ref<module_manager> init(module_manager_props props);

        module_manager(module_manager_props props);
        ~module_manager();

        bool load_modules();
        bool load_module(std::filesystem::directory_entry entry);

        void trigger_event(const std::string& event_name, const std::string& data);

        ref<module> get_module_by_source(const std::filesystem::path& source);
    private:
        bool check_lua(int r);

        lua_State *m_lua_state = nullptr;
        module_manager_props m_props;
        std::vector<ref<module>> m_modules;
        std::vector<ref<module>> m_pending_modules;
        ref<module_event_manager> m_event_manager;
    };
}