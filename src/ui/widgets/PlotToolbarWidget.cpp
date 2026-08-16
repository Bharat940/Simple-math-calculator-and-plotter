#include "ui/widgets/PlotToolbarWidget.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/UIManager.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "imgui.h"
#include <algorithm>

namespace mathstudio::ui
{

    void PlotToolbarWidget::render(core::EvaluationContext &)
    {
        ImGui::TextDisabled("CANVAS OVERLAYS & CONTROLS");
        ImGui::Separator();
        ImGui::Spacing();

        float availW = ImGui::GetContentRegionAvail().x;

        ImGui::Checkbox("Show Grid [G]", &showGrid);
        if (availW > 240.0f)
            ImGui::SameLine();
        ImGui::Checkbox("Show Roots [R]", &showRoots);

        ImGui::Checkbox("Show Extrema [E]", &showExtrema);
        if (availW > 240.0f)
            ImGui::SameLine();
        ImGui::Checkbox("Show Tangent [T]", &showTangent);

        ImGui::Checkbox("Show Derivative [D]", &showDerivative);
        if (availW > 240.0f)
            ImGui::SameLine();
        ImGui::Checkbox("Canvas Text Labels", &showLabels);
        ImGui::Spacing();

        if (ImGui::TreeNode("Global Marker & Tangent Colors"))
        {
            ImGui::Checkbox("Match Curve Color", &matchFunctionColor);
            if (!matchFunctionColor)
            {
                ImGui::Text("Roots Color");
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::ColorEdit3("##RootsColor", rootColor, ImGuiColorEditFlags_NoInputs);

                ImGui::Text("Extrema Color");
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::ColorEdit3("##ExtremaColor", extremaColor, ImGuiColorEditFlags_NoInputs);
            }
            ImGui::Text("Tangent Color");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::ColorEdit3("##TangentColor", tangentColor, ImGuiColorEditFlags_NoInputs);

            ImGui::Text("Derivative Curve Color");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::ColorEdit3("##DerivColor", derivativeColor, ImGuiColorEditFlags_NoInputs);
            ImGui::TreePop();
        }
    }

} // namespace mathstudio::ui
