#pragma once

namespace mathstudio::ui
{

    /**
     * @brief MathStudio Obsidian Slate Design System
     *
     * Clean, restrained, professional desktop aesthetic.
     * Muted gray background, slate panels, soft borders (1px),
     * and rounded corners (5px - 7px).
     */
    struct DesignSystem
    {
        // Style Parameters
        static constexpr float WindowRounding = 6.0f;
        static constexpr float FrameRounding = 5.0f;
        static constexpr float GrabRounding = 5.0f;
        static constexpr float PopupRounding = 6.0f;
        static constexpr float ScrollbarRounding = 6.0f;
        static constexpr float ChildRounding = 5.0f;

        static constexpr float WindowBorderSize = 1.0f;
        static constexpr float FrameBorderSize = 0.0f;
        static constexpr float PopupBorderSize = 1.0f;

        // Color Palette Tokens (RGBA Float 0.0 - 1.0)
        struct ColorToken
        {
            float r, g, b, a;
        };

        // Preset 1: Obsidian Slate (Default Dark)
        static constexpr ColorToken ObsidianBg = {0.078f, 0.086f, 0.110f, 1.0f};     // #14161c
        static constexpr ColorToken ObsidianPanel = {0.102f, 0.114f, 0.149f, 1.0f};  // #1a1d26
        static constexpr ColorToken ObsidianHeader = {0.149f, 0.165f, 0.212f, 1.0f}; // #262a36
        static constexpr ColorToken ObsidianBorder = {0.180f, 0.200f, 0.250f, 1.0f};
        static constexpr ColorToken ObsidianAccent = {0.310f, 0.639f, 0.890f, 1.0f}; // #4fa3e3
        static constexpr ColorToken ObsidianAccentHover = {0.400f, 0.700f, 0.950f, 1.0f};
        static constexpr ColorToken ObsidianText = {0.878f, 0.902f, 0.929f, 1.0f}; // #e0e6ed
        static constexpr ColorToken ObsidianTextMuted = {0.550f, 0.600f, 0.650f, 1.0f};
    };

} // namespace mathstudio::ui
