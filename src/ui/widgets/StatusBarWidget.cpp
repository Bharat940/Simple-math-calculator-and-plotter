#include "ui/widgets/StatusBarWidget.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void StatusBarWidget::render(core::EvaluationContext &)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 24.0f));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 24.0f));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 2.0f));
        if (ImGui::Begin("##StatusBar", nullptr, flags))
        {
            ImGui::Text("Cursor: (%.3f, %.3f)", mouseX, mouseY);
            ImGui::SameLine(250);
            ImGui::Text("Engine: AST Pratt Compiler");
            ImGui::SameLine(500);
            ImGui::TextDisabled("Press Ctrl+P for Command Palette");
            ImGui::End();
        }
        ImGui::PopStyleVar();
    }

} // namespace mathstudio::ui
