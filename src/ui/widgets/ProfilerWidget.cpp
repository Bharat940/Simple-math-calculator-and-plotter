#include "ui/widgets/ProfilerWidget.hpp"
#include "core/performance/FrameProfiler.hpp"
#include "imgui.h"
#include <cstdio>

namespace mathstudio::ui
{
    void ProfilerWidget::render(core::EvaluationContext &)
    {
        auto &profiler = core::performance::FrameProfiler::instance();
        const auto &telemetry = profiler.getTelemetry();
        const auto &zones = profiler.getZones();

        ImGui::TextDisabled("FRAME PACING & LATENCY DISTRIBUTION");
        ImGui::Columns(4, "LatencyCols", false);
        ImGui::Text("Current: %.2f ms", telemetry.frameTimeMs);
        ImGui::NextColumn();
        ImGui::Text("FPS: %.1f", telemetry.fps);
        ImGui::NextColumn();
        ImGui::Text("Median (P50): %.2f ms", telemetry.p50Ms);
        ImGui::NextColumn();
        ImGui::Text("P95: %.2f ms", telemetry.p95Ms);
        ImGui::NextColumn();
        ImGui::Columns(1);

        ImGui::TextDisabled("Min: %.2f ms | P99: %.2f ms | Max: %.2f ms (over rolling 300 frames)",
                            telemetry.minMs, telemetry.p99Ms, telemetry.maxMs);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextDisabled("SUBSYSTEM EXECUTION BREAKDOWN");
        double totalFrameMs = telemetry.frameTimeMs > 0.0 ? telemetry.frameTimeMs : (telemetry.p50Ms > 0.0 ? telemetry.p50Ms : 16.667);

        if (ImGui::BeginTable("SubsystemBreakdownTable", 4,
                              ImGuiTableFlags_BordersInnerH |
                                  ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollX))
        {
            ImGui::TableSetupColumn("Subsystem", ImGuiTableColumnFlags_WidthFixed, 175.0f);
            ImGui::TableSetupColumn("Last (ms)", ImGuiTableColumnFlags_WidthFixed, 85.0f);
            ImGui::TableSetupColumn("Avg (ms)", ImGuiTableColumnFlags_WidthFixed, 85.0f);
            ImGui::TableSetupColumn("Load Share", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < zones.size(); ++i)
            {
                const auto &z = zones[i];
                if (!z.name)
                    continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(z.name);

                ImGui::TableNextColumn();
                ImGui::Text("%.3f ms", z.lastFrameMs);

                ImGui::TableNextColumn();
                ImGui::Text("%.3f ms", z.avgMs);

                ImGui::TableNextColumn();
                float fraction = static_cast<float>(z.avgMs / totalFrameMs);
                if (fraction > 1.0f)
                    fraction = 1.0f;
                char pctText[32];
                std::snprintf(pctText, sizeof(pctText), "%.1f%%", fraction * 100.0f);
                ImGui::ProgressBar(fraction, ImVec2(-1, 14), pctText);
            }
            ImGui::EndTable();
        }

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextDisabled("OBSERVABLE VERIFICATION COUNTERS");

        if (telemetry.astEvaluations == 0)
        {
            ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.0f), "AST Evaluations / Frame   : 0 (Zero Idle Math)");
        }
        else
        {
            ImGui::Text("AST Evaluations / Frame   : %u", telemetry.astEvaluations);
        }

        ImGui::Text("Plot Points Sampled / Frame: %u pts", telemetry.plotPointsSampled);

        uint32_t totalCache = telemetry.cacheHits + telemetry.cacheMisses;
        double hitRatio = totalCache > 0 ? (100.0 * telemetry.cacheHits / totalCache) : 100.0;
        ImGui::Text("Expression Cache Hits     : %.1f%% (%u hits, %u misses)", hitRatio, telemetry.cacheHits, telemetry.cacheMisses);

#if defined(MATHSTUDIO_ENABLE_ALLOCATION_PROFILING) && (MATHSTUDIO_ENABLE_ALLOCATION_PROFILING == 1)
        ImGui::Text("Heap Allocations / Frame  : %u", telemetry.heapAllocations);
#endif
    }

} // namespace mathstudio::ui
