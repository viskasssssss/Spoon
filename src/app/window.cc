#include "window.hh"

#include <string>
#include <glad/glad.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include "assert.hh"
#include "event/app_event.hh"
#include "event/key_event.hh"
#include "event/mouse_event.hh"

namespace spoon {
    static bool sdl_initialized = false;
    static bool glad_initialized = false;

    int window_resize_filter(void* userdata, SDL_Event* event)
    {
        if (event->type == SDL_WINDOWEVENT &&
            (event->window.event == SDL_WINDOWEVENT_RESIZED ||
            event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
        {
            window* win = static_cast<window*>(userdata);

            int width = event->window.data1;
            int height = event->window.data2;

            if (width > 0 && height > 0)
            {
                glViewport(0, 0, width, height);
                win->render_frame();
            }
        }

        return 1;
    }

    window::window(window_props props) {
        // make sure that sdl initialized
        if (!sdl_initialized) {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                std::string message = std::string("Couldn't initialize SDL2; SDL_Error: ") + SDL_GetError();
                SP_ASSERT(false, message);
                m_is_valid = false;
                return;
            }
            sdl_initialized = true;
        }

        // set opengl specific attributes

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        m_flags = props.flags;

        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        if (props.flags & WINDOW_FLAG_RESIZABLE) flags |= SDL_WINDOW_RESIZABLE;

        // create window

        m_handle = SDL_CreateWindow(
            props.title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            props.width, props.height,
            flags
        );

        // make sure that the window is correctly initialized

        if (m_handle == nullptr) {
            SP_CORE_ERROR("Window couldn't be created; SDL_Error: {0}", SDL_GetError());
            SDL_Quit();
            m_is_valid = false;
            return;
        }

        // create opengl context

        m_gl_context = SDL_GL_CreateContext(m_handle);
        if (!m_gl_context) {
            SP_CORE_ERROR("OpenGL context couldn't be created; SDL_Error: {0}", SDL_GetError());
            SDL_DestroyWindow(m_handle);
            SDL_Quit();
            m_is_valid = false;
            return;
        }

        SDL_GL_MakeCurrent(m_handle, m_gl_context);

        // make sure that glad is initialized

        if (!glad_initialized) {
            if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
                SP_CORE_ERROR("Failed to initialize GLAD");

                SDL_GL_DeleteContext(m_gl_context);
                SDL_DestroyWindow(m_handle);
                SDL_Quit();

                m_is_valid = false;
                return;
            }

            glad_initialized = true;
        }

        // verify that m_is_valid and m_is_closed are correct

        m_is_valid = true;
        m_is_closed = false;

        // setup imgui and imgui platform/renderer backends

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_imgui_io = &ImGui::GetIO(); (void)m_imgui_io;

        if (!(m_flags & WINDOW_FLAG_DISABLE_DOCKING)) {
            m_imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            m_imgui_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  
        }

        ImGui_ImplSDL2_InitForOpenGL(m_handle, m_gl_context);
        ImGui_ImplOpenGL3_Init("#version 130");

        // set the event filter

        SDL_SetEventFilter(window_resize_filter, this);

    }

    window::~window() {
        SDL_GL_DeleteContext(m_gl_context);
        SDL_DestroyWindow(m_handle);
        SDL_Quit();

        m_is_valid = false;
    }

    void window::poll_events() {

        if (!m_event_callback || !m_is_valid) return;

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type) {
                case SDL_QUIT: {
                    window_close_event event;
                    m_event_callback(event);
                    m_is_closed = true;
                    break;
                }
                case SDL_WINDOWEVENT: { 
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED || 
                        event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        
                        int width = event.window.data1;
                        int height = event.window.data2;

                        glViewport(0, 0, width, height);

                        window_resize_event event(width, height);
                        m_event_callback(event);
                    }
                    break;
                }
                case SDL_KEYDOWN: {
                    SDL_Scancode scancode = event.key.keysym.scancode;
                    SDL_Keycode key = SDL_GetKeyFromScancode(scancode);
                    key_pressed_event event(key);
                    m_event_callback(event);
                    break;
                }
                case SDL_KEYUP: {
                    SDL_Scancode scancode = event.key.keysym.scancode;
                    SDL_Keycode key = SDL_GetKeyFromScancode(scancode);
                    key_released_event event(key);
                    m_event_callback(event);
                    break;
                }
                case SDL_TEXTINPUT: {
                    char c = event.text.text[0];
                    char charString[2] = { c, '\0' };
    
                    SDL_Keycode key = SDL_GetKeyFromName(charString);
                    key_typed_event event(key);
                    m_event_callback(event);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void window::render_frame()
    {
        if (!m_is_valid) return;

        render_start();

        if (m_render_callback)
            m_render_callback();

        render_end();
    }

    void window::render_start() {

        // start new imgui frame

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (m_imgui_io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    }

    void window::render_end() {
        if (!m_is_valid) return;

        // render imgui

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // handle multi-windows
        if (m_imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {

            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

        }

        // make sure that the window updates

        swap_buffers();
    }

    void window::swap_buffers() {
        if (!m_is_valid) return;

        SDL_GL_SwapWindow(m_handle);
    }
}