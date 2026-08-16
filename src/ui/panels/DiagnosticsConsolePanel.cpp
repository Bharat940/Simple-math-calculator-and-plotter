#include "ui/panels/DiagnosticsConsolePanel.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void DiagnosticsConsolePanel::draw(core::EvaluationContext &context)
    {
        if (ImGui::Begin(getTitle(), &isOpen))
        {
            consoleWidget.render(context);
        }
        ImGui::End();
    }

} // namespace mathstudio::ui
