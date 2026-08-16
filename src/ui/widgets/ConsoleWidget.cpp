#include "ui/widgets/ConsoleWidget.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    void ConsoleWidget::render(core::EvaluationContext &context)
    {
        if (!ImGui::BeginTabBar("DiagnosticsTabs"))
            return;

        // ═══════════════════════════════════════════════════════════════════
        //  ERRORS
        // ═══════════════════════════════════════════════════════════════════
        if (ImGui::BeginTabItem("Errors"))
        {
            const auto &errors = context.diagnostics.getErrors();

            if (!errors.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
                ImGui::Text("  %zu error(s) detected", errors.size());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                if (ImGui::SmallButton("Clear##Err"))
                    context.diagnostics.clearErrors();
                ImGui::Separator();
                ImGui::Spacing();

                for (size_t i = 0; i < errors.size(); ++i)
                {
                    const auto &err = errors[i];
                    ImGui::PushID(static_cast<int>(i));

                    // Badge
                    ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "[E%zu]", i + 1);
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.45f, 1.0f), "%s", err.message.c_str());
                    if (err.column > 0)
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(col %zu)", err.column);
                    }

                    // Caret under the offending column
                    if (err.column > 0 && !err.message.empty() && err.message[0] == '[')
                    {
                        size_t close = err.message.find(']');
                        if (close != std::string::npos && close > 1)
                        {
                            std::string exprStr = err.message.substr(1, close - 1);
                            ImGui::TextDisabled("      %s", exprStr.c_str());
                            std::string caret(err.column > 0 ? err.column - 1 : 0, ' ');
                            caret += '^';
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.2f, 1.0f), "      %s", caret.c_str());
                        }
                    }

                    if (!err.hint.empty())
                        ImGui::TextColored(ImVec4(0.75f, 0.75f, 0.35f, 1.0f),
                                           "      Hint: %s", err.hint.c_str());

                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.0f), "  No errors detected.");
                ImGui::TextDisabled("  All active expressions parsed successfully.");
            }
            ImGui::EndTabItem();
        }

        // ═══════════════════════════════════════════════════════════════════
        //  WARNINGS
        // ═══════════════════════════════════════════════════════════════════
        if (ImGui::BeginTabItem("Warnings"))
        {
            const auto &warnings = context.diagnostics.getWarnings();

            if (!warnings.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.25f, 1.0f));
                ImGui::Text("  %zu warning(s)", warnings.size());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                if (ImGui::SmallButton("Clear##Warn"))
                    context.diagnostics.clearWarnings();
                ImGui::Separator();
                ImGui::Spacing();

                for (size_t i = 0; i < warnings.size(); ++i)
                {
                    const auto &w = warnings[i];
                    ImGui::PushID(static_cast<int>(i + 1000));

                    ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.20f, 1.0f), "[W%zu]", i + 1);
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.90f, 0.45f, 1.0f), "%s", w.message.c_str());

                    if (!w.source.empty())
                    {
                        ImGui::TextDisabled("      Expression: %s", w.source.c_str());
                    }
                    if (!w.hint.empty())
                    {
                        ImGui::TextColored(ImVec4(0.65f, 0.90f, 0.55f, 1.0f),
                                           "      Hint: %s", w.hint.c_str());
                    }

                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.0f), "  No warnings.");
                ImGui::TextDisabled("  NaN / domain / overflow warnings will appear here.");
            }
            ImGui::EndTabItem();
        }

        // ═══════════════════════════════════════════════════════════════════
        //  TIMING
        // ═══════════════════════════════════════════════════════════════════
        if (ImGui::BeginTabItem("Timing"))
        {
            const auto &timings = context.diagnostics.getTimings();

            ImGui::Spacing();

            if (!timings.empty())
            {
                // Column header
                ImGui::Separator();
                ImGui::TextDisabled("  %-42s  %10s", "Stage", "Time (ms)");
                ImGui::Separator();

                double total = 0.0;
                for (const auto &t : timings)
                {
                    // Highlight rows that are above 1ms in amber
                    if (t.ms >= 1.0)
                        ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.30f, 1.0f),
                                           "  %-42s  %10.4f", t.label.c_str(), t.ms);
                    else
                        ImGui::Text("  %-42s  %10.4f", t.label.c_str(), t.ms);
                    total += t.ms;
                }

                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.5f, 0.85f, 1.0f, 1.0f),
                                   "  %-42s  %10.4f", "TOTAL", total);
                ImGui::Spacing();

                // Visual bar chart (normalized to max)
                double maxMs = 0.0;
                for (const auto &t : timings)
                    if (t.ms > maxMs)
                        maxMs = t.ms;

                if (maxMs > 0.0)
                {
                    ImGui::Separator();
                    ImGui::TextDisabled("  Visual profile (proportional bars):");
                    ImGui::Spacing();

                    float availW = ImGui::GetContentRegionAvail().x - 20.0f;
                    for (const auto &t : timings)
                    {
                        float frac = static_cast<float>(t.ms / maxMs);
                        float barW = frac * availW;
                        ImVec4 barCol = (t.ms >= 1.0)
                                            ? ImVec4(1.0f, 0.65f, 0.20f, 0.80f)
                                            : ImVec4(0.25f, 0.65f, 1.0f, 0.70f);

                        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barCol);
                        ImGui::ProgressBar(frac, ImVec2(barW, 14.0f), "");
                        ImGui::PopStyleColor();
                        ImGui::SameLine();
                        ImGui::TextDisabled("%.3f ms  %s", t.ms, t.label.c_str());
                    }
                }
            }
            else
            {
                ImGui::TextDisabled("  Timing data will appear here once a function is plotted.");
                ImGui::Spacing();
                ImGui::TextDisabled("  Stages tracked:");
                ImGui::TextDisabled("    Parse:   tokenize + PrattParser AST build");
                ImGui::TextDisabled("    Eval:    600-sample f(x) point evaluation loop");
                ImGui::TextDisabled("    Total:   combined pipeline time");
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

} // namespace mathstudio::ui
