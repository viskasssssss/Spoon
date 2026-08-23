#include "app.hh"

#include <glad/glad.h>
#include <imgui.h>

#include "def.hh"
#include "log.hh"
#include "event/app_event.hh"

namespace spoon {
    app::app() {
        log::init();

        window_props wprops{};
        wprops.title = "Spoon Editor";
        wprops.width = 1280;
        wprops.height = 720;
        wprops.flags = WINDOW_FLAG_RESIZABLE;

        m_main_window = create_ref<window>(wprops);
        m_main_window->set_event_callback(SP_BIND_EVENT_FN(app::on_event));
        m_main_window->set_render_callback(SP_BIND_EVENT_FN(app::on_render));

        // initialize the module manager

        module_manager_props mprops{};
        m_module_manager = create_ref<module_manager>(mprops);
    }

    app::~app() {}

    void app::run() {
        m_running = true;
        while (m_running) {
            m_main_window->poll_events();

            m_main_window->render_frame();
        }
    }

    void app::on_event(event& e) {
        event_dispatcher dispatcher(e);
		dispatcher.dispatch<window_close_event>(SP_BIND_EVENT_FN(app::on_window_close));
    }

    bool app::on_window_close(window_close_event& e) {
        e.handled = true;
        m_running = false;
        return true;
    }

    void app::on_render() {
        ImGui::Begin("Test Window!");

        ImGui::Text("Hello! I'm the 'Test Window'! Nice to meet you.");

        ImGui::End();
            
        glClearColor(0.1f, 0.4f, 0.5f, 1.0f);
            
        glClear(GL_COLOR_BUFFER_BIT);
    }
}