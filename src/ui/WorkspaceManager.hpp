#pragma once

#include <vector>
#include <string>

namespace mathstudio::ui
{

    enum class WorkspacePreset
    {
        Default,
        Scientific,
        DSP,
        Calculus,
        Minimal,
        ResetLayout
    };

    class WorkspaceManager
    {
    public:
        static WorkspaceManager &instance();

        void applyPreset(WorkspacePreset preset);
        WorkspacePreset getCurrentPreset() const { return m_currentPreset; }
        const char *getPresetName(WorkspacePreset preset) const;

        std::vector<WorkspacePreset> getAvailablePresets() const;

    private:
        WorkspaceManager() = default;
        WorkspacePreset m_currentPreset = WorkspacePreset::Default;
    };

} // namespace mathstudio::ui
