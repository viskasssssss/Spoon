#pragma once

#include <string>
#include <functional>

#include <string>
#include <SDL2/SDL.h>
#include <imgui.h>

#include "event/event.hh"

namespace spoon {
    enum window_flags {
        WINDOW_FLAG_RESIZABLE            = 1 << 0,
        WINDOW_FLAG_DISABLE_DOCKING      = 1 << 2
    };

    struct window_props {
        const char* title;
        unsigned int width;
        unsigned int height;
        Uint32 flags;
    };

    class window {
    public:
        using event_callback_fn = std::function<void(event&)>;
        using render_callback_fn = std::function<void()>;

        window(window_props props);
        ~window();

        void poll_events();
        void render_frame();
        void set_event_callback(const event_callback_fn& callback) { m_event_callback = callback; }
        void set_render_callback(const render_callback_fn& callback) { m_render_callback = callback; }

        bool is_valid() const { return m_is_valid; }
        bool is_closed() const { return m_is_closed; }
    protected:
        void render_start();
        void swap_buffers();
        void render_end();
    private:
        bool m_is_closed = false;
        bool m_is_valid = false;
        Uint32 m_flags = 0;
        SDL_Window* m_handle;
        SDL_GLContext m_gl_context;
        ImGuiIO* m_imgui_io = nullptr;
        event_callback_fn m_event_callback;
        render_callback_fn m_render_callback;
    };
}