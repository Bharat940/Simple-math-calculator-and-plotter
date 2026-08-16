#include "ui/widgets/ProfilerWidget.hpp"
#include "core/EngineProfiler.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void ProfilerWidget::render(core::EvaluationContext &)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto &stats = core::EngineProfilerStats::instance();

        float fps = io.Framerate > 0.0f ? io.Framerate : 60.0f;
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", fps, 1000.0f / fps);
        ImGui::Separator();

        ImGui::TextDisabled("DYNAMIC AST ENGINE PROFILER");
        ImGui::Text("Uncached Parse Time    : %.3f ms", stats.lastParseTimeMs);
        ImGui::Text("ConstantFold Pass Time : %.3f ms", stats.lastConstantFoldTimeMs);
        ImGui::Text("SimplifyVisitor Time   : %.3f ms", stats.lastSimplifyTimeMs);

        double usPerPt = stats.lastPlotPointsCount > 0 ? (stats.lastPlotEvalTimeMs * 1000.0) / static_cast<double>(stats.lastPlotPointsCount) : 0.0;
        ImGui::Text("Live Frame Plot Eval   : %.3f ms (%d pts, ~%.2f us/pt)", stats.lastPlotEvalTimeMs, stats.lastPlotPointsCount, usPerPt);
        ImGui::Separator();

        ImGui::TextDisabled("MEMORY & CACHE METRICS");
        size_t totalLookups = stats.totalCacheHits + stats.totalCacheMisses;
        double hitRatio = totalLookups > 0 ? (100.0 * static_cast<double>(stats.totalCacheHits) / static_cast<double>(totalLookups)) : 100.0;
        ImGui::Text("Expression Cache Hits  : %.1f%% (%zu hits, %zu misses)", hitRatio, stats.totalCacheHits, stats.totalCacheMisses);
        ImGui::Text("AST Node Enum Overhead : 1 byte compact opcodes");
    }

} // namespace mathstudio::ui
