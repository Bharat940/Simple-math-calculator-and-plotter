#include "ui/CommandPalette.hpp"
#include "ui/ThemeManager.hpp"
#include "ui/WorkspaceManager.hpp"
#include "imgui.h"
#include <algorithm>
#include <cctype>

namespace mathstudio::ui
{

    CommandPalette &CommandPalette::instance()
    {
        static CommandPalette inst;
        return inst;
    }

    CommandPalette::CommandPalette()
    {
        populateDefaultCommands();
    }

    void CommandPalette::registerCommand(const CommandAction &action)
    {
        m_actions.push_back(action);
    }

    void CommandPalette::open()
    {
        m_isOpen = true;
        m_searchQuery[0] = '\0';
    }

    void CommandPalette::close()
    {
        m_isOpen = false;
    }

    void CommandPalette::populateDefaultCommands()
    {
        m_actions = {
            {"Reset Viewport Camera", "Plotter", "R", [](core::EvaluationContext &) {}},
            {"Toggle Roots Callouts", "Plotter", "R", [](core::EvaluationContext &) {}},
            {"Toggle Extrema Callouts", "Plotter", "E", [](core::EvaluationContext &) {}},
            {"Toggle Tangent Inspection", "Plotter", "T", [](core::EvaluationContext &) {}},
            {"Toggle Grid Display", "Plotter", "G", [](core::EvaluationContext &) {}},
            {"Switch Theme: Dark Obsidian", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::DarkObsidian); }},
            {"Switch Theme: Light Clean", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::LightClean); }},
            {"Switch Theme: Dracula", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::Dracula); }},
            {"Switch Theme: Solarized Dark", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::SolarizedDark); }},
            {"Switch Theme: VS Code Dark", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::VSCodeDark); }},
            {"Switch Theme: MATLAB Classic", "Themes", "", [](core::EvaluationContext &)
             { ThemeManager::instance().applyTheme(ThemePreset::MatlabClassic); }},
            {"Workspace: Default", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::Default); }},
            {"Workspace: Scientific", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::Scientific); }},
            {"Workspace: DSP & Signals", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::DSP); }},
            {"Workspace: Calculus", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::Calculus); }},
            {"Workspace: Minimal", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::Minimal); }},
            {"Workspace: Reset Layout", "Workspaces", "", [](core::EvaluationContext &)
             { WorkspaceManager::instance().applyPreset(WorkspacePreset::ResetLayout); }}};
    }

    void CommandPalette::render(core::EvaluationContext &context)
    {
        if (!m_isOpen)
            return;

        ImGui::OpenPopup("Command Palette (Ctrl+P)");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f));

        if (ImGui::BeginPopupModal("Command Palette (Ctrl+P)", &m_isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            if (ImGui::IsWindowAppearing())
            {
                ImGui::SetKeyboardFocusHere();
            }

            ImGui::InputText("##Search", m_searchQuery, IM_ARRAYSIZE(m_searchQuery));
            ImGui::Separator();

            std::string queryStr = m_searchQuery;
            std::transform(queryStr.begin(), queryStr.end(), queryStr.begin(), [](unsigned char c)
                           { return std::tolower(c); });

            ImGui::BeginChild("ActionList", ImVec2(0, 0), true);
            for (const auto &action : m_actions)
            {
                std::string actionLower = action.name;
                std::transform(actionLower.begin(), actionLower.end(), actionLower.begin(), [](unsigned char c)
                               { return std::tolower(c); });

                if (queryStr.empty() || actionLower.find(queryStr) != std::string::npos)
                {
                    std::string label = "[" + action.category + "] " + action.name;
                    if (ImGui::Selectable(label.c_str()))
                    {
                        if (action.handler)
                        {
                            action.handler(context);
                        }
                        close();
                    }
                }
            }
            ImGui::EndChild();

            ImGui::EndPopup();
        }
    }

} // namespace mathstudio::ui
