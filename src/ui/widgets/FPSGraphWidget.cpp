#include "ui/widgets/FPSGraphWidget.hpp"
#include "core/performance/FrameProfiler.hpp"
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
    void FPSGraphWidget::render(core::EvaluationContext &)
    {
        auto &profiler = core::performance::FrameProfiler::instance();
        const auto &history = profiler.getFrameHistory();
        int ringIdx = static_cast<int>(profiler.getRingIndex());
        const auto &telemetry = profiler.getTelemetry();

        // Render frame time history using ImGui PlotLines directly from pre-allocated ring buffer
        char overlayText[64];
        std::snprintf(overlayText, sizeof(overlayText), "%.2f ms (%.1f FPS)", telemetry.frameTimeMs, telemetry.fps);

        ImGui::PlotLines("##FrameTimePlot", history.data(), static_cast<int>(history.size()), ringIdx, overlayText, 0.0f, 33.33f, ImVec2(-1, 65));

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
