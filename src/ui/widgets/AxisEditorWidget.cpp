#include "ui/widgets/AxisEditorWidget.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/UIManager.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "imgui.h"

namespace mathstudio::ui
{

    void AxisEditorWidget::render(core::EvaluationContext &)
    {
        ImGui::TextDisabled(ICON_FA_SLIDERS " VIEWPORT & AXIS BOUNDARIES");
        ImGui::Separator();
        ImGui::Spacing();

        float availW = ImGui::GetContentRegionAvail().x;

        // ── X Axis (Horizontal Viewport Bounds) ──
        ImGui::TextDisabled(ICON_FA_ARROWS_LEFT_RIGHT " HORIZONTAL AXIS (X-VIEWPORT)");

        ImGui::Text("X Min (Left Boundary)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::DragFloat("##XMin", &xMin, 0.1f, -500.0f, xMax - 0.5f, "Left (X Min): %.1f");

        ImGui::Spacing();
        ImGui::Text("X Max (Right Boundary)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::DragFloat("##XMax", &xMax, 0.1f, xMin + 0.5f, 500.0f, "Right (X Max): %.1f");

        ImGui::Spacing();

        // ── Y Axis (Vertical Viewport Bounds) ──
        ImGui::TextDisabled(ICON_FA_ARROWS_UP_DOWN " VERTICAL AXIS (Y-VIEWPORT)");

        ImGui::Text("Y Min (Bottom Boundary)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::DragFloat("##YMin", &yMin, 0.1f, -500.0f, yMax - 0.5f, "Bottom (Y Min): %.1f");

        ImGui::Spacing();
        ImGui::Text("Y Max (Top Boundary)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::DragFloat("##YMax", &yMax, 0.1f, yMin + 0.5f, 500.0f, "Top (Y Max): %.1f");

        ImGui::Spacing();

        // ── Quick Range Presets ──
        float btnW = (availW - 8.0f) * 0.5f;
        if (btnW < 80.0f)
            btnW = availW;

        if (ImGui::Button("Standard [-10, 10]", ImVec2(btnW, 0)))
        {
            xMin = -10.0f;
            xMax = 10.0f;
            yMin = -6.0f;
            yMax = 6.0f;
        }
        if (btnW < availW)
            ImGui::SameLine();
        if (ImGui::Button("Trig [-2pi, 2pi]", ImVec2(btnW, 0)))
        {
            xMin = -6.283f;
            xMax = 6.283f;
            yMin = -2.0f;
            yMax = 2.0f;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Grid Scaling ──
        ImGui::Text("Grid Ticks Scaling");
        static const char *gridModes[] = {"Auto (Adaptive)", "Fixed (Step 1.0)", "Loose (Step 5.0)", "Dense (Step 0.5)"};
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::Combo("##GridModeCombo", &gridMode, gridModes, IM_ARRAYSIZE(gridModes));

        ImGui::Spacing();

        // ── Asymptote Threshold ──
        ImGui::Text("Asymptote Threshold (Pole Clipping)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderFloat("##DiscThreshold", &discThreshold, 5.0f, 1000.0f, "Threshold: %.0f");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Clips steep vertical asymptote connections for functions with poles like tan(x) or 1/x.");
        }

        ImGui::Spacing();
        if (ImGui::TreeNode("Test Asymptote Presets"))
        {
            ControlPanel *ctrl = UIManager::instance().getPanel<ControlPanel>();
            float presetW = (availW - 10.0f) / 3.0f;
            if (presetW < 60.0f)
                presetW = availW;

            if (ImGui::Button("tan(x)", ImVec2(presetW, 0)))
            {
                if (ctrl)
                {
                    ctrl->exprWidget.clear();
                    ctrl->exprWidget.addExpression("tan(x)");
                    discThreshold = 25.0f;
                }
            }
            if (presetW < availW)
                ImGui::SameLine();
            if (ImGui::Button("1/x", ImVec2(presetW, 0)))
            {
                if (ctrl)
                {
                    ctrl->exprWidget.clear();
                    ctrl->exprWidget.addExpression("1/x");
                    discThreshold = 25.0f;
                }
            }
            if (presetW < availW)
                ImGui::SameLine();
            if (ImGui::Button("1/(x-2)", ImVec2(presetW, 0)))
            {
                if (ctrl)
                {
                    ctrl->exprWidget.clear();
                    ctrl->exprWidget.addExpression("1/(x-2)");
                    discThreshold = 25.0f;
                }
            }
            ImGui::TreePop();
        }
    }

} // namespace mathstudio::ui
