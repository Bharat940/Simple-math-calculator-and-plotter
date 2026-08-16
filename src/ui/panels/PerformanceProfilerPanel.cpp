#include "ui/panels/PerformanceProfilerPanel.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void PerformanceProfilerPanel::draw(core::EvaluationContext &context)
    {
        if (ImGui::Begin(getTitle(), &isOpen))
        {
            profilerWidget.render(context);
            ImGui::Spacing();
            fpsWidget.render(context);
        }
        ImGui::End();
    }

} // namespace mathstudio::ui
