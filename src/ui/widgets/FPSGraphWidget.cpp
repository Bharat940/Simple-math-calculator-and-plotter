#include "ui/widgets/FPSGraphWidget.hpp"
#include "imgui.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

namespace mathstudio::ui
{

    FPSGraphWidget::FPSGraphWidget()
    {
        m_frameTimeHistory.resize(m_maxHistory, 16.6f);
    }

    void FPSGraphWidget::render(core::EvaluationContext &)
    {
        float fps = ImGui::GetIO().Framerate > 0.0f ? ImGui::GetIO().Framerate : 60.0f;
        float currentMs = 1000.0f / fps;

        m_frameTimeHistory.erase(m_frameTimeHistory.begin());
        m_frameTimeHistory.push_back(currentMs);

        ImGui::PlotLines("Frame Time (ms)", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0.0f, 33.3f, ImVec2(0, 80));

#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        {
            double ramMb = static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
            ImGui::TextColored(ImVec4(0.4f, 0.85f, 0.4f, 1.0f), "Live Memory (RAM): %.2f MB", ramMb);
        }
#elif defined(__unix__) || defined(__APPLE__)
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0)
        {
#if defined(__APPLE__)
            double ramMb = static_cast<double>(usage.ru_maxrss) / (1024.0 * 1024.0);
#else
            double ramMb = static_cast<double>(usage.ru_maxrss) / 1024.0;
#endif
            ImGui::TextColored(ImVec4(0.4f, 0.85f, 0.4f, 1.0f), "Live Memory (RAM): %.2f MB", ramMb);
        }
#endif
    }

} // namespace mathstudio::ui
