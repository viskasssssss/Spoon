#include "module_context.hh"

#include <imgui/imgui.h>

#include "app/window.hh"
#include "log.hh"
#include "def.hh"

#include "module_context.hh"
#include "module.hh"

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
        case UI_INSTRUCTION_BUTTON: {
            if (args.size() != 2) { SP_CORE_ERROR("Expected 2 arguments but got {0}", args.size()); return; }

            bool result = ImGui::Button(args[0].value.c_str());

            // if button is pressed, call the event
            if (result) {
                uint64_t id = 0;

                // converting id string to uint64_T
                try {
                    id = std::stoull(args[1].value); 
                } 
                catch (const std::invalid_argument& e) {
                    SP_CORE_ERROR("Invalid argument. Expected string containing 64-bit integer (UUID) but got {0}", args[1].value.c_str());
                    return;
                } 
                catch (const std::out_of_range& e) {
                    SP_CORE_ERROR("Invalid UUID. The value is too large for a 64-bit integer (UUID)");
                    return;
                }

                // trigger the button event
                context.get_manager()->trigger_event("ui.button", std::to_string(id));
            }
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
        if (args[0] == "button") {
            if (args.size() != 3) { SP_CORE_ERROR("Expected 3 arguments, but got {0}", args.size()); return {};}
            return { UI_INSTRUCTION_BUTTON, { {args[1]}, {args[2]} } };
        }
        SP_CORE_ERROR("Unknown instruction ""{0}"" ", args[0]);
        return {};
    }
}