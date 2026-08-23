#pragma once

#include "window.hh"
#include "event/app_event.hh"


namespace spoon {
    class app {
    public:
        app();
        ~app();

        void run();
        
        void on_event(event& e);
        bool on_window_close(window_close_event& e);
        void on_render();
    private:
        bool m_running = true;
        ref<window> m_main_window = nullptr;
    };
}