#include "ui/panels/VariableInspectorPanel.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void VariableInspectorPanel::draw(core::EvaluationContext &context)
    {
        if (ImGui::Begin(getTitle(), &isOpen))
        {
            tableWidget.render(context);
        }
        ImGui::End();
    }

} // namespace mathstudio::ui
