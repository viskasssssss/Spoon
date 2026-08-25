#pragma once

#include <string>
#include <vector>

#include "uuid.hh"
#include "registry.hh"
#include "app/window.hh"

namespace spoon {

    class module_context;

    enum ui_instruction_type {
        UI_INSTRUCTION_START_WINDOW,
        UI_INSTRUCTION_END_WINDOW,
        UI_INSTRUCTION_TEXT,
        UI_INSTRUCTION_BUTTON
    };

    struct ui_argument {
        std::string value;
    };

    struct ui_instruction {
        ui_instruction_type type;
        std::vector<ui_argument> args;

        void execute(module_context& context);
        static ui_instruction create_from_arguments(std::vector<std::string> args);
    };
}