#include "module_context.hh"

#include <imgui/imgui.h>

#include "app/window.hh"
#include "log.hh"
#include "def.hh"

#include "module_context.hh"

namespace spoon {
    void ui_instruction::execute(module_context& context) {
        switch (type) {
        case UI_INSTRUCTION_START_WINDOW: {
            if (args.size() != 1) { SP_CORE_ERROR("Expected 1 argument but got {0}", args.size()); return; }

            ImGui::Begin(args[0].value.c_str());
            
            break;
        }
        case UI_INSTRUCTION_END_WINDOW: {
            if (args.size() != 0) { SP_CORE_ERROR("Expected 0 arguments but got {0}", args.size()); return; }

            ImGui::End();

            break;
        }
        case UI_INSTRUCTION_TEXT: {
            if (args.size() != 1) { SP_CORE_ERROR("Expected 1 argument but got {0}", args.size()); return; }

            ImGui::Text(args[0].value.c_str());

            break;
        }
        default: {
            SP_CORE_WARN("No implementation found for given instruction type");
            break;
        }
        }
    }

    ui_instruction ui_instruction::create_from_arguments(std::vector<std::string> args) {
        if (args.size() < 1) { SP_CORE_ERROR("Expected atleast 1 argument, but got {0}", args.size()); return {}; }
        if (args[0] == "text") {
            if (args.size() != 2) { SP_CORE_ERROR("Expected 2 arguments, but got {0}", args.size()); return {};}
            return { UI_INSTRUCTION_TEXT, { {args[1]} } };
        }
        SP_CORE_ERROR("Unknown instruction ""{0}"" ", args[0]);
        return {};
    }
}