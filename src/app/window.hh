#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include <string>
#include <SDL2/SDL.h>
#include <imgui.h>

#include "event/event.hh"
#include "uuid.hh"
#include "registry.hh"

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
        size_t set_event_callback(const event_callback_fn& callback) { m_event_callbacks->clear(); return m_event_callbacks->register_function(callback); }
        size_t set_render_callback(const render_callback_fn& callback) { m_render_callbacks->clear(); return m_render_callbacks->register_function(callback); }
        size_t add_event_callback(const event_callback_fn& callback) { return m_event_callbacks->register_function(callback); }
        size_t add_render_callback(const render_callback_fn& callback) { return m_render_callbacks->register_function(callback); }
        ref<function_registry<event&>> get_event_callbacks() { return m_event_callbacks; }
        ref<function_registry<>> get_render_callbacks() { return m_render_callbacks; }

        bool is_valid() const { return m_is_valid; }
        bool is_closed() const { return m_is_closed; }

        uuid get_id() const { return m_id; }

        void call_event_callbacks(event& e);
        void call_render_callbacks();

        static std::vector<window*> get_all_windows();
        static window* get_window_by_uuid(uuid id);
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
        ref<function_registry<event&>> m_event_callbacks = nullptr;
        ref<function_registry<>> m_render_callbacks = nullptr;
        uuid m_id;
    };
}