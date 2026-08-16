#include "ui/panels/ControlPanel.hpp"
#include "imgui.h"
#include <algorithm>

namespace mathstudio::ui
{

    void ControlPanel::draw(core::EvaluationContext &context)
    {
        // Enforce industry-standard minimum/maximum window dimensions (VS Code / MATLAB sidebar standard: 280px min width)
        ImGui::SetNextWindowSizeConstraints(ImVec2(280.0f, 300.0f), ImVec2(700.0f, 2000.0f));

        if (ImGui::Begin(getTitle(), &isOpen, ImGuiWindowFlags_HorizontalScrollbar))
        {
            // Enable text wrapping across the control panel to prevent text clipping
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + std::max(200.0f, ImGui::GetContentRegionAvail().x));

            if (ImGui::CollapsingHeader("Expressions", ImGuiTreeNodeFlags_DefaultOpen))
            {
                exprWidget.render(context);
            }

            if (ImGui::CollapsingHeader("Plot & Axis Settings", ImGuiTreeNodeFlags_DefaultOpen))
            {
                axisWidget.render(context);
            }

            if (ImGui::CollapsingHeader("Canvas Overlays & Markers"))
            {
                toolbarWidget.render(context);
            }

            ImGui::Spacing();
            ImGui::Separator();

            float btnWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::CollapsingHeader("Calculus Engine (v0.4.0 Roadmap)"))
            {
                ImGui::TextDisabled("Symbolic Differentiation & Numerical Integration");
                ImGui::BeginDisabled();
                ImGui::Button("Compute d/dx (v0.4.0)", ImVec2(btnWidth, 0));
                ImGui::Button("Compute Integral ∫ (v0.4.0)", ImVec2(btnWidth, 0));
                ImGui::Button("Taylor Series (v0.4.0)", ImVec2(btnWidth, 0));
                ImGui::EndDisabled();
            }

            if (ImGui::CollapsingHeader("DSP & Signal Processing (v0.5.0 Roadmap)"))
            {
                ImGui::TextDisabled("DFT / FFT Cooley-Tukey & Filter Design");
                ImGui::BeginDisabled();
                ImGui::Button("1024-Point FFT (v0.5.0)", ImVec2(btnWidth, 0));
                ImGui::Button("FIR Filter (v0.5.0)", ImVec2(btnWidth, 0));
                ImGui::Button("Aliasing Sim (v0.5.0)", ImVec2(btnWidth, 0));
                ImGui::EndDisabled();
            }

            if (ImGui::CollapsingHeader("Linear Algebra (v0.6.0 Roadmap)"))
            {
                ImGui::TextDisabled("Matrix Operations & Linear System Solvers");
                ImGui::BeginDisabled();
                ImGui::Button("Solve [A]{x}={b} (v0.6.0)", ImVec2(btnWidth, 0));
                ImGui::Button("Eigenvalues (v0.6.0)", ImVec2(btnWidth, 0));
                ImGui::EndDisabled();
            }

            ImGui::PopTextWrapPos();
        }
        ImGui::End();
    }

} // namespace mathstudio::ui
