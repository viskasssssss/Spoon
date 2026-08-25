#include "module_context.hh"

#include <imgui/imgui.h>

#include "app/window.hh"
#include "log.hh"
#include "def.hh"

namespace spoon {
    void module_context::set_callback() {
        if (m_current_callback_idx != -1) {
            SP_CORE_WARN("set_callback(): callback already exists, not creating another");
            return;
        }

        window* window =
            window::get_window_by_uuid(m_ui_window);

        if (!window) {
            SP_CORE_ERROR("Could not find UI window");
            return;
        }

        m_current_callback_idx =
            window->add_render_callback([this]() {

                for (auto& instruction : m_pending_instructions) {
                    instruction.execute(*this);
                }

                m_pending_instructions.clear();
            });
    }

    void module_context::delete_callback(bool silent) {
        if (m_current_callback_idx != -1) {
            window* window = window::get_window_by_uuid(m_ui_window);
            if (!window) {
                SP_CORE_ERROR("Could not find UI window");
                return;
            }

            window->get_render_callbacks()->remove_function(m_current_callback_idx);
            m_current_callback_idx = -1;
            return;
        }
        if (!silent)
            SP_CORE_WARN("Current callback doesn't set");
    }

    void module_context::mark_callback_for_deletion() {
        if (m_current_callback_idx != -1) {
            window* window = window::get_window_by_uuid(m_ui_window);
            if (!window) {
                SP_CORE_ERROR("Could not find UI window");
                return;
            }

            window->get_render_callbacks()->mark_for_deletion(m_current_callback_idx);

            m_current_callback_idx = -1;
            return;
        }
        SP_CORE_WARN("Current callback doesn't set");
    }
}