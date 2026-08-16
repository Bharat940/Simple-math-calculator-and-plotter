#pragma once

#include <vector>
#include <string>

namespace mathstudio::ui
{

    enum class ThemePreset
    {
        DarkObsidian,
        LightClean,
        Dracula,
        SolarizedDark,
        VSCodeDark,
        MatlabClassic
    };

    class ThemeManager
    {
    public:
        static ThemeManager &instance();

        void applyTheme(ThemePreset preset);
        ThemePreset getCurrentPreset() const { return m_currentPreset; }
        const char *getPresetName(ThemePreset preset) const;

        std::vector<ThemePreset> getAvailablePresets() const;

    private:
        ThemeManager() = default;
        ThemePreset m_currentPreset = ThemePreset::DarkObsidian;
    };

} // namespace mathstudio::ui
