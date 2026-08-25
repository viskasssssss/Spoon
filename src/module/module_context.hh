#pragma once

#include <string>
#include <vector>

#include "uuid.hh"
#include "registry.hh"
#include "app/window.hh"

#include "module_instruction.hh"

namespace spoon {
    class module_manager;

    class module_context {
    public:
        module_context(module_manager* manager);

        uuid get_current_ui_window_id() const { return m_ui_window; }
        void set_current_ui_window_id(uuid id) { m_ui_window = id; }

        bool is_started_window() const { return m_is_started_window; }
        void set_is_started_window(bool b) { m_is_started_window = b; }

        void add_to_pending_instructions(ui_instruction instruction) { m_pending_instructions.push_back(instruction); }

        module_manager* get_manager() { return m_manager; }

        void set_callback();
        void delete_callback(bool silent = false);
        void mark_callback_for_deletion();
    private:
        uuid m_ui_window = 0;
        bool m_is_started_window = false;
        std::vector<ui_instruction> m_pending_instructions;
        int m_current_callback_idx = -1;
        module_manager* m_manager = nullptr;
    };
}