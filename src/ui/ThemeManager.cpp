#include "ui/ThemeManager.hpp"
#include "ui/DesignSystem.hpp"
#include "imgui.h"
#include "implot.h"

namespace mathstudio::ui
{

    ThemeManager &ThemeManager::instance()
    {
        static ThemeManager inst;
        return inst;
    }

    const char *ThemeManager::getPresetName(ThemePreset preset) const
    {
        switch (preset)
        {
        case ThemePreset::DarkObsidian:
            return "Dark Obsidian";
        case ThemePreset::LightClean:
            return "Light Clean";
        case ThemePreset::Dracula:
            return "Dracula";
        case ThemePreset::SolarizedDark:
            return "Solarized Dark";
        case ThemePreset::VSCodeDark:
            return "VS Code Dark";
        case ThemePreset::MatlabClassic:
            return "MATLAB Classic";
        }
        return "Dark Obsidian";
    }

    std::vector<ThemePreset> ThemeManager::getAvailablePresets() const
    {
        return {
            ThemePreset::DarkObsidian,
            ThemePreset::LightClean,
            ThemePreset::Dracula,
            ThemePreset::SolarizedDark,
            ThemePreset::VSCodeDark,
            ThemePreset::MatlabClassic};
    }

    void ThemeManager::applyTheme(ThemePreset preset)
    {
        m_currentPreset = preset;
        if (!ImGui::GetCurrentContext())
            return;

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

        // Apply MathStudio Obsidian Slate Rounding & Spacing Standards
        style.WindowRounding = DesignSystem::WindowRounding;
        style.FrameRounding = DesignSystem::FrameRounding;
        style.GrabRounding = DesignSystem::GrabRounding;
        style.PopupRounding = DesignSystem::PopupRounding;
        style.ScrollbarRounding = DesignSystem::ScrollbarRounding;
        style.ChildRounding = DesignSystem::ChildRounding;

        style.WindowBorderSize = DesignSystem::WindowBorderSize;
        style.FrameBorderSize = DesignSystem::FrameBorderSize;
        style.PopupBorderSize = DesignSystem::PopupBorderSize;

        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);

        switch (preset)
        {
        case ThemePreset::DarkObsidian:
        {
            colors[ImGuiCol_WindowBg] = ImVec4(0.070f, 0.082f, 0.110f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.094f, 0.110f, 0.149f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.094f, 0.110f, 0.149f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.180f, 0.200f, 0.260f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.125f, 0.149f, 0.212f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.180f, 0.220f, 0.300f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.220f, 0.270f, 0.380f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.094f, 0.110f, 0.149f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.125f, 0.149f, 0.212f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.094f, 0.110f, 0.149f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.070f, 0.082f, 0.110f, 0.50f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.180f, 0.220f, 0.300f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.000f, 0.667f, 1.000f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.000f, 0.667f, 1.000f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.000f, 0.667f, 1.000f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.200f, 0.733f, 1.000f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.125f, 0.149f, 0.212f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.000f, 0.533f, 0.800f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.000f, 0.667f, 1.000f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.125f, 0.149f, 0.212f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.000f, 0.533f, 0.800f, 1.00f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.000f, 0.667f, 1.000f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.094f, 0.110f, 0.149f, 1.00f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.180f, 0.220f, 0.300f, 1.00f);
            colors[ImGuiCol_TabActive] = ImVec4(0.125f, 0.149f, 0.212f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.933f, 0.949f, 0.969f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.420f, 0.447f, 0.502f, 1.00f);
            break;
        }

        case ThemePreset::LightClean:
        {
            ImGui::StyleColorsLight();
            colors[ImGuiCol_WindowBg] = ImVec4(0.973f, 0.980f, 0.988f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.796f, 0.835f, 0.882f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.929f, 0.949f, 0.969f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.886f, 0.910f, 0.941f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.796f, 0.835f, 0.882f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.929f, 0.949f, 0.969f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.886f, 0.910f, 0.941f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.929f, 0.949f, 0.969f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.886f, 0.910f, 0.941f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.145f, 0.388f, 0.922f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.118f, 0.314f, 0.745f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.145f, 0.388f, 0.922f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.145f, 0.388f, 0.922f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.886f, 0.910f, 0.941f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.796f, 0.835f, 0.882f, 1.00f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.145f, 0.388f, 0.922f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.929f, 0.949f, 0.969f, 1.00f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.796f, 0.835f, 0.882f, 1.00f);
            colors[ImGuiCol_TabActive] = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.059f, 0.090f, 0.165f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.278f, 0.333f, 0.412f, 1.00f);
            break;
        }

        case ThemePreset::Dracula:
        {
            ImGui::StyleColorsDark();
            colors[ImGuiCol_WindowBg] = ImVec4(0.118f, 0.122f, 0.161f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.157f, 0.165f, 0.212f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.157f, 0.165f, 0.212f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.267f, 0.278f, 0.353f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.267f, 0.278f, 0.353f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.388f, 0.439f, 0.643f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.741f, 0.576f, 0.976f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.267f, 0.278f, 0.353f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.388f, 0.439f, 0.643f, 1.00f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.741f, 0.576f, 0.976f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.267f, 0.278f, 0.353f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.388f, 0.439f, 0.643f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.741f, 0.576f, 0.976f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.314f, 0.980f, 0.482f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.741f, 0.576f, 0.976f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.475f, 0.776f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.973f, 0.973f, 0.949f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.545f, 0.580f, 0.706f, 1.00f);
            break;
        }

        case ThemePreset::SolarizedDark:
        {
            ImGui::StyleColorsDark();
            colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.169f, 0.212f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.027f, 0.212f, 0.259f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.027f, 0.212f, 0.259f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.035f, 0.263f, 0.322f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.035f, 0.263f, 0.322f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.043f, 0.278f, 0.337f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.165f, 0.631f, 0.596f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.035f, 0.263f, 0.322f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.165f, 0.631f, 0.596f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.035f, 0.263f, 0.322f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.165f, 0.631f, 0.596f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.165f, 0.631f, 0.596f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.165f, 0.631f, 0.596f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.514f, 0.580f, 0.588f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.345f, 0.431f, 0.459f, 1.00f);
            break;
        }

        case ThemePreset::VSCodeDark:
        {
            ImGui::StyleColorsDark();
            colors[ImGuiCol_WindowBg] = ImVec4(0.118f, 0.118f, 0.118f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.145f, 0.145f, 0.145f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.145f, 0.145f, 0.145f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.235f, 0.235f, 0.235f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.235f, 0.235f, 0.235f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.000f, 0.478f, 0.800f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.000f, 0.478f, 0.800f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.100f, 0.550f, 0.900f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.000f, 0.478f, 0.800f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.000f, 0.478f, 0.800f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.831f, 0.831f, 0.831f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.00f);
            break;
        }

        case ThemePreset::MatlabClassic:
        {
            ImGui::StyleColorsLight();
            colors[ImGuiCol_WindowBg] = ImVec4(0.937f, 0.937f, 0.937f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
            colors[ImGuiCol_Border] = ImVec4(0.700f, 0.700f, 0.700f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.886f, 0.886f, 0.886f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.800f, 0.850f, 0.950f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.800f, 0.850f, 0.950f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.000f, 0.294f, 0.529f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.000f, 0.294f, 0.529f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.000f, 0.400f, 0.700f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.000f, 0.294f, 0.529f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.000f, 0.294f, 0.529f, 1.00f);
            colors[ImGuiCol_Text] = ImVec4(0.000f, 0.000f, 0.000f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.300f, 0.300f, 0.300f, 1.00f);
            break;
        }
        }

        // Synchronize ImPlot visual styles with selected UI theme
        if (ImPlot::GetCurrentContext())
        {
            if (preset == ThemePreset::LightClean || preset == ThemePreset::MatlabClassic)
            {
                ImPlot::StyleColorsLight();
                ImPlotStyle &pStyle = ImPlot::GetStyle();
                pStyle.Colors[ImPlotCol_PlotBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                pStyle.Colors[ImPlotCol_PlotBorder] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
                pStyle.Colors[ImPlotCol_AxisGrid] = ImVec4(0.15f, 0.20f, 0.28f, 0.30f);
                pStyle.Colors[ImPlotCol_AxisText] = ImVec4(0.059f, 0.090f, 0.165f, 1.00f);
                pStyle.Colors[ImPlotCol_TitleText] = ImVec4(0.059f, 0.090f, 0.165f, 1.00f);
            }
            else
            {
                ImPlot::StyleColorsDark();
                ImPlotStyle &pStyle = ImPlot::GetStyle();
                pStyle.Colors[ImPlotCol_PlotBg] = ImVec4(0.070f, 0.082f, 0.110f, 1.00f);
                pStyle.Colors[ImPlotCol_AxisGrid] = ImVec4(0.80f, 0.85f, 0.90f, 0.25f);
                pStyle.Colors[ImPlotCol_AxisText] = ImVec4(0.933f, 0.949f, 0.969f, 1.00f);
                pStyle.Colors[ImPlotCol_TitleText] = ImVec4(0.933f, 0.949f, 0.969f, 1.00f);
            }
        }
    }

} // namespace mathstudio::ui
