#include "ui/WorkspaceManager.hpp"
#include "ui/UIManager.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/panels/VariableInspectorPanel.hpp"
#include "ui/panels/PerformanceProfilerPanel.hpp"
#include "ui/panels/DiagnosticsConsolePanel.hpp"
#include "ui/panels/PlotPanel.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace mathstudio::ui
{

    WorkspaceManager &WorkspaceManager::instance()
    {
        static WorkspaceManager inst;
        return inst;
    }

    const char *WorkspaceManager::getPresetName(WorkspacePreset preset) const
    {
        switch (preset)
        {
        case WorkspacePreset::Default:
            return "Default Workspace";
        case WorkspacePreset::Scientific:
            return "Scientific Computing";
        case WorkspacePreset::DSP:
            return "DSP & Signals";
        case WorkspacePreset::Calculus:
            return "Calculus Analysis";
        case WorkspacePreset::Minimal:
            return "Minimal Plotter";
        case WorkspacePreset::ResetLayout:
            return "Reset Dock Layout";
        }
        return "Default Workspace";
    }

    std::vector<WorkspacePreset> WorkspaceManager::getAvailablePresets() const
    {
        return {
            WorkspacePreset::Default,
            WorkspacePreset::Scientific,
            WorkspacePreset::DSP,
            WorkspacePreset::Calculus,
            WorkspacePreset::Minimal,
            WorkspacePreset::ResetLayout};
    }

    void WorkspaceManager::applyPreset(WorkspacePreset preset)
    {
        m_currentPreset = preset;
        if (!ImGui::GetCurrentContext())
            return;

        ImGuiID dockspaceID = ImGui::GetID("MathStudioDockSpaceNode");

        // Remove existing layout nodes to build fresh preset layout
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->WorkSize);

        ImGuiID dockMainID = dockspaceID;
        ImGuiID dockLeftID = 0;
        ImGuiID dockRightID = 0;
        ImGuiID dockBottomID = 0;

        ControlPanel *ctrl = UIManager::instance().getPanel<ControlPanel>();
        VariableInspectorPanel *vars = UIManager::instance().getPanel<VariableInspectorPanel>();
        PerformanceProfilerPanel *prof = UIManager::instance().getPanel<PerformanceProfilerPanel>();
        DiagnosticsConsolePanel *diag = UIManager::instance().getPanel<DiagnosticsConsolePanel>();
        PlotPanel *plot = UIManager::instance().getPanel<PlotPanel>();

        if (ctrl)
            ctrl->isOpen = true;
        if (plot)
            plot->isOpen = true;

        switch (preset)
        {
        case WorkspacePreset::Default:
        case WorkspacePreset::ResetLayout:
        {
            if (vars)
                vars->isOpen = true;
            if (prof)
                prof->isOpen = true;
            if (diag)
                diag->isOpen = true;

            // 1. Split Left Sidebar (30% width) for Control Panel & Variable Inspector
            dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.30f, nullptr, &dockMainID);

            // 2. Split Bottom Tray (25% height) for Diagnostics & Performance Profiler
            dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);

            // 3. Split Left Sidebar vertically: Top Left = Control Panel, Bottom Left = Variable Inspector
            ImGuiID dockLeftBottomID = 0;
            ImGui::DockBuilderSplitNode(dockLeftID, ImGuiDir_Down, 0.50f, &dockLeftBottomID, &dockLeftID);

            // Dock windows with exact title strings
            ImGui::DockBuilderDockWindow(ICON_FA_SLIDERS " Control Panel", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_TABLE " Variable Inspector", dockLeftBottomID);
            ImGui::DockBuilderDockWindow(ICON_FA_TERMINAL " Diagnostics Console", dockBottomID);
            ImGui::DockBuilderDockWindow(ICON_FA_GAUGE " Performance Profiler", dockBottomID);
            ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Scientific Plotter", dockMainID);
            break;
        }

        case WorkspacePreset::Scientific:
        {
            if (vars)
                vars->isOpen = true;
            if (prof)
                prof->isOpen = false;
            if (diag)
                diag->isOpen = true;

            dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.28f, nullptr, &dockMainID);
            dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.28f, nullptr, &dockMainID);

            ImGui::DockBuilderDockWindow(ICON_FA_SLIDERS " Control Panel", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_TABLE " Variable Inspector", dockRightID);
            ImGui::DockBuilderDockWindow(ICON_FA_TERMINAL " Diagnostics Console", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Scientific Plotter", dockMainID);
            break;
        }

        case WorkspacePreset::DSP:
        {
            if (vars)
                vars->isOpen = true;
            if (prof)
                prof->isOpen = true;
            if (diag)
                diag->isOpen = false;

            dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.30f, nullptr, &dockMainID);
            dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.30f, nullptr, &dockMainID);

            ImGui::DockBuilderDockWindow(ICON_FA_SLIDERS " Control Panel", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_TABLE " Variable Inspector", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_GAUGE " Performance Profiler", dockBottomID);
            ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Scientific Plotter", dockMainID);
            break;
        }

        case WorkspacePreset::Calculus:
        {
            if (vars)
                vars->isOpen = true;
            if (prof)
                prof->isOpen = false;
            if (diag)
                diag->isOpen = true;

            dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.30f, nullptr, &dockMainID);
            dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);

            ImGui::DockBuilderDockWindow(ICON_FA_SLIDERS " Control Panel", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_TABLE " Variable Inspector", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_TERMINAL " Diagnostics Console", dockBottomID);
            ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Scientific Plotter", dockMainID);
            break;
        }

        case WorkspacePreset::Minimal:
        {
            if (vars)
                vars->isOpen = false;
            if (prof)
                prof->isOpen = false;
            if (diag)
                diag->isOpen = false;

            dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.22f, nullptr, &dockMainID);
            ImGui::DockBuilderDockWindow(ICON_FA_SLIDERS " Control Panel", dockLeftID);
            ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Scientific Plotter", dockMainID);
            break;
        }
        }

        ImGui::DockBuilderFinish(dockspaceID);
    }

} // namespace mathstudio::ui
