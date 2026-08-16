#include "ui/panels/PlotPanel.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/UIManager.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "math/expression.h"
#include "math/solver.h"
#include "math/numerical.h"
#include "core/EngineProfiler.hpp"
#include "imgui.h"
#include "implot.h"
#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include <chrono>

namespace mathstudio::ui
{

    // ─── Helpers ──────────────────────────────────────────────────────────────
    static double nowMs()
    {
        using namespace std::chrono;
        return static_cast<double>(
                   duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count()) /
               1000.0;
    }

    // Simple DFT for power spectrum rendering
    static void computePowerSpectrum(const std::vector<double> &timeSignal, double sampleRate,
                                     std::vector<double> &freqs, std::vector<double> &psd)
    {
        size_t N = timeSignal.size();
        size_t numFreqs = N / 2;
        freqs.resize(numFreqs);
        psd.resize(numFreqs);

        for (size_t k = 0; k < numFreqs; ++k)
        {
            freqs[k] = (k * sampleRate) / static_cast<double>(N);
            double re = 0.0, im = 0.0;
            for (size_t n = 0; n < N; ++n)
            {
                double angle = 2.0 * 3.14159265358979323846 * k * n / static_cast<double>(N);
                re += timeSignal[n] * std::cos(angle);
                im -= timeSignal[n] * std::sin(angle);
            }
            psd[k] = (re * re + im * im) / static_cast<double>(N);
        }
    }

    // ─── draw ─────────────────────────────────────────────────────────────────
    void PlotPanel::draw(core::EvaluationContext &context)
    {
        if (!ImGui::Begin(getTitle(), &isOpen))
        {
            ImGui::End();
            return;
        }

        ControlPanel *ctrl = UIManager::instance().getPanel<ControlPanel>();

        // ── Keyboard Shortcuts ──────────────────────────────────────────────
        if (ctrl && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_G))
                ctrl->toolbarWidget.showGrid = !ctrl->toolbarWidget.showGrid;
            if (ImGui::IsKeyPressed(ImGuiKey_T))
                ctrl->toolbarWidget.showTangent = !ctrl->toolbarWidget.showTangent;
            if (ImGui::IsKeyPressed(ImGuiKey_R))
                ctrl->toolbarWidget.showRoots = !ctrl->toolbarWidget.showRoots;
            if (ImGui::IsKeyPressed(ImGuiKey_E))
                ctrl->toolbarWidget.showExtrema = !ctrl->toolbarWidget.showExtrema;
            if (ImGui::IsKeyPressed(ImGuiKey_D))
                ctrl->toolbarWidget.showDerivative = !ctrl->toolbarWidget.showDerivative;

            float panStepX = (ctrl->axisWidget.xMax - ctrl->axisWidget.xMin) * 0.05f;
            float panStepY = (ctrl->axisWidget.yMax - ctrl->axisWidget.yMin) * 0.05f;

            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            {
                ctrl->axisWidget.xMin -= panStepX;
                ctrl->axisWidget.xMax -= panStepX;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            {
                ctrl->axisWidget.xMin += panStepX;
                ctrl->axisWidget.xMax += panStepX;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            {
                ctrl->axisWidget.yMin += panStepY;
                ctrl->axisWidget.yMax += panStepY;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            {
                ctrl->axisWidget.yMin -= panStepY;
                ctrl->axisWidget.yMax -= panStepY;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Home))
            {
                ctrl->axisWidget.xMin = -10.0f;
                ctrl->axisWidget.xMax = 10.0f;
                ctrl->axisWidget.yMin = -6.0f;
                ctrl->axisWidget.yMax = 6.0f;
            }

            auto zoomIn = [&]()
            {
                float cx = (ctrl->axisWidget.xMin + ctrl->axisWidget.xMax) * 0.5f;
                float cy = (ctrl->axisWidget.yMin + ctrl->axisWidget.yMax) * 0.5f;
                float rx = (ctrl->axisWidget.xMax - ctrl->axisWidget.xMin) * 0.4f;
                float ry = (ctrl->axisWidget.yMax - ctrl->axisWidget.yMin) * 0.4f;
                ctrl->axisWidget.xMin = cx - rx;
                ctrl->axisWidget.xMax = cx + rx;
                ctrl->axisWidget.yMin = cy - ry;
                ctrl->axisWidget.yMax = cy + ry;
            };
            auto zoomOut = [&]()
            {
                float cx = (ctrl->axisWidget.xMin + ctrl->axisWidget.xMax) * 0.5f;
                float cy = (ctrl->axisWidget.yMin + ctrl->axisWidget.yMax) * 0.5f;
                float rx = (ctrl->axisWidget.xMax - ctrl->axisWidget.xMin) * 0.6f;
                float ry = (ctrl->axisWidget.yMax - ctrl->axisWidget.yMin) * 0.6f;
                ctrl->axisWidget.xMin = cx - rx;
                ctrl->axisWidget.xMax = cx + rx;
                ctrl->axisWidget.yMin = cy - ry;
                ctrl->axisWidget.yMax = cy + ry;
            };

            if (ImGui::IsKeyPressed(ImGuiKey_Equal) ||
                ImGui::IsKeyPressed(ImGuiKey_KeypadAdd) ||
                ImGui::IsKeyPressed(ImGuiKey_PageUp))
                zoomIn();

            if (ImGui::IsKeyPressed(ImGuiKey_Minus) ||
                ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract) ||
                ImGui::IsKeyPressed(ImGuiKey_PageDown))
                zoomOut();
        }

        double xMin = -10.0, xMax = 10.0;
        double yMin = -6.0, yMax = 6.0;

        if (ctrl)
        {
            xMin = static_cast<double>(ctrl->axisWidget.xMin);
            xMax = static_cast<double>(ctrl->axisWidget.xMax);
            yMin = static_cast<double>(ctrl->axisWidget.yMin);
            yMax = static_cast<double>(ctrl->axisWidget.yMax);
        }

        // ── Single-Row Modern Master Plotter Toolbar ───────────────────────
        if (ctrl)
        {
            if (ImGui::Button(ICON_FA_ROTATE_LEFT " Reset View"))
            {
                ctrl->axisWidget.xMin = -10.0f;
                ctrl->axisWidget.xMax = 10.0f;
                ctrl->axisWidget.yMin = -6.0f;
                ctrl->axisWidget.yMax = 6.0f;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Reset Viewport Camera to Default [-10, 10] (Home Key)");

            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_PLUS " Zoom In"))
            {
                float cx = (ctrl->axisWidget.xMin + ctrl->axisWidget.xMax) * 0.5f;
                float cy = (ctrl->axisWidget.yMin + ctrl->axisWidget.yMax) * 0.5f;
                float rx = (ctrl->axisWidget.xMax - ctrl->axisWidget.xMin) * 0.4f;
                float ry = (ctrl->axisWidget.yMax - ctrl->axisWidget.yMin) * 0.4f;
                ctrl->axisWidget.xMin = cx - rx;
                ctrl->axisWidget.xMax = cx + rx;
                ctrl->axisWidget.yMin = cy - ry;
                ctrl->axisWidget.yMax = cy + ry;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Zoom Camera In (+ / PageUp Key)");

            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_MINUS " Zoom Out"))
            {
                float cx = (ctrl->axisWidget.xMin + ctrl->axisWidget.xMax) * 0.5f;
                float cy = (ctrl->axisWidget.yMin + ctrl->axisWidget.yMax) * 0.5f;
                float rx = (ctrl->axisWidget.xMax - ctrl->axisWidget.xMin) * 0.6f;
                float ry = (ctrl->axisWidget.yMax - ctrl->axisWidget.yMin) * 0.6f;
                ctrl->axisWidget.xMin = cx - rx;
                ctrl->axisWidget.xMax = cx + rx;
                ctrl->axisWidget.yMin = cy - ry;
                ctrl->axisWidget.yMax = cy + ry;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Zoom Camera Out (- / PageDown Key)");

            ImGui::SameLine();
            ImGui::TextDisabled("|");
            ImGui::SameLine();

            ImGui::Checkbox(ICON_FA_BORDER_ALL " Grid [G]", &ctrl->toolbarWidget.showGrid);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle Canvas Grid (Press G)");

            ImGui::SameLine();
            ImGui::Checkbox(ICON_FA_CIRCLE_DOT " Roots [R]", &ctrl->toolbarWidget.showRoots);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle Root Markers (Press R)");

            ImGui::SameLine();
            ImGui::Checkbox(ICON_FA_LOCATION_DOT " Extrema [E]", &ctrl->toolbarWidget.showExtrema);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle Extrema Markers (Press E)");

            ImGui::SameLine();
            ImGui::Checkbox(ICON_FA_VECTOR_SQUARE " Tangent [T]", &ctrl->toolbarWidget.showTangent);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle Mouse Tangent Line (Press T)");

            ImGui::SameLine();
            ImGui::Checkbox(ICON_FA_CHART_LINE " Deriv [D]", &ctrl->toolbarWidget.showDerivative);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle Derivative Curve f'(x) (Press D)");

            ImGui::Spacing();
        }

        ImPlotFlags flags = ImPlotFlags_Crosshairs;
        ImPlotAxisFlags axisFlags = (ctrl && !ctrl->toolbarWidget.showGrid)
                                        ? ImPlotAxisFlags_NoGridLines
                                        : ImPlotAxisFlags_None;

        if (ImGui::BeginTabBar("PlotTabs"))
        {
            // ── Cartesian f(x) ────────────────────────────────────────────
            if (ImGui::BeginTabItem(ICON_FA_CHART_LINE " Cartesian f(x)"))
            {
                ImPlot::SetNextAxesLimits(xMin, xMax, yMin, yMax, ImPlotCond_Always);

                if (ImPlot::BeginPlot("##CartesianPlotter", ImVec2(-1, -1), flags))
                {
                    ImPlot::SetupAxes("x", "f(x)", axisFlags, axisFlags);

                    if (ctrl)
                    {
                        // Apply Grid Scaling Modes (0: Auto, 1: Fixed 1.0, 2: Loose 5.0, 3: Dense 0.5)
                        int gMode = ctrl->axisWidget.gridMode;
                        if (gMode == 1) // Fixed 1.0
                        {
                            ImPlot::SetupAxisTicks(ImAxis_X1, xMin, xMax, static_cast<int>(std::abs(xMax - xMin)) + 1);
                            ImPlot::SetupAxisTicks(ImAxis_Y1, yMin, yMax, static_cast<int>(std::abs(yMax - yMin)) + 1);
                        }
                        else if (gMode == 2) // Loose 5.0
                        {
                            ImPlot::SetupAxisTicks(ImAxis_X1, xMin, xMax, static_cast<int>(std::abs(xMax - xMin) / 5.0) + 1);
                            ImPlot::SetupAxisTicks(ImAxis_Y1, yMin, yMax, static_cast<int>(std::abs(yMax - yMin) / 5.0) + 1);
                        }
                        else if (gMode == 3) // Dense 0.5
                        {
                            ImPlot::SetupAxisTicks(ImAxis_X1, xMin, xMax, static_cast<int>(std::abs(xMax - xMin) * 2.0) + 1);
                            ImPlot::SetupAxisTicks(ImAxis_Y1, yMin, yMax, static_cast<int>(std::abs(yMax - yMin) * 2.0) + 1);
                        }

                        const auto &items = ctrl->exprWidget.getExpressions();
                        constexpr int SAMPLE_COUNT = 600;

                        static std::vector<double> xData(SAMPLE_COUNT);
                        for (int i = 0; i < SAMPLE_COUNT; ++i)
                            xData[i] = xMin + (xMax - xMin) * i / static_cast<double>(SAMPLE_COUNT - 1);

                        double discThresh = static_cast<double>(ctrl->axisWidget.discThreshold);

                        context.diagnostics.clearErrors();
                        context.diagnostics.clearWarnings();

                        double totalEvalMs = 0.0;
                        int totalSamples = 0;

                        for (size_t exprIdx = 0; exprIdx < items.size(); ++exprIdx)
                        {
                            const auto &item = items[exprIdx];
                            if (!item.enabled || item.text.empty())
                                continue;

                            double t0parse = nowMs();
                            Expression expr(item.text, context.diagnostics, item.text);
                            double parseMs = nowMs() - t0parse;

                            context.diagnostics.recordTiming("Parse: " + item.text, parseMs);

                            if (!expr.isValid())
                                continue;

                            static std::vector<double> yData(SAMPLE_COUNT);
                            int nanCount = 0;

                            double t0eval = nowMs();
                            for (int i = 0; i < SAMPLE_COUNT; ++i)
                            {
                                context.vars.set("x", MathValue::real(xData[i]));
                                double y = expr.evalWithContext(xData[i], context);

                                // Enhanced Asymptote / Discontinuity clipping
                                if (i > 0 && std::isfinite(y) && std::isfinite(yData[i - 1]))
                                {
                                    double diff = std::abs(y - yData[i - 1]);
                                    bool isSignFlipNearPole = (y * yData[i - 1] < 0) && (diff > discThresh * 0.1);
                                    if (diff > discThresh || isSignFlipNearPole)
                                    {
                                        yData[i - 1] = std::numeric_limits<double>::quiet_NaN();
                                    }
                                }

                                if (!std::isfinite(y))
                                    ++nanCount;

                                yData[i] = std::isfinite(y) ? y : std::numeric_limits<double>::quiet_NaN();
                            }
                            double evalMs = nowMs() - t0eval;
                            totalEvalMs += evalMs;
                            totalSamples += SAMPLE_COUNT;

                            auto &profStats = core::EngineProfilerStats::instance();
                            profStats.lastParseTimeMs = parseMs;
                            profStats.lastPlotEvalTimeMs = totalEvalMs;
                            profStats.lastPlotPointsCount = totalSamples;
                            profStats.totalCacheHits++;

                            context.diagnostics.recordTiming(
                                "Eval (" + std::to_string(SAMPLE_COUNT) + " pts): " + item.text, evalMs);

                            if (nanCount > 0)
                            {
                                int pct = (nanCount * 100) / SAMPLE_COUNT;
                                context.diagnostics.reportWarning(
                                    "Domain warning: " + std::to_string(nanCount) +
                                        " sample(s) (" + std::to_string(pct) + "%) produced NaN / undefined",
                                    item.text,
                                    "Check for sqrt of negative, log of non-positive, or division by zero");
                            }

                            std::string lineLabel = item.text + "##" + std::to_string(exprIdx);
                            ImVec4 col(item.color[0], item.color[1], item.color[2], 1.0f);
                            ImPlot::SetNextLineStyle(col, 2.0f);
                            ImPlot::PlotLine(lineLabel.c_str(), xData.data(), yData.data(), SAMPLE_COUNT);

                            if (ctrl->toolbarWidget.showDerivative && item.showDerivative)
                            {
                                std::vector<double> dyData(SAMPLE_COUNT);
                                for (int i = 0; i < SAMPLE_COUNT; ++i)
                                {
                                    double dy = derivative(expr, xData[i], 1e-5, &context);
                                    dyData[i] = std::isfinite(dy) ? dy : std::numeric_limits<double>::quiet_NaN();
                                }
                                std::string derivLabel = "d/dx[" + item.text + "]##Deriv" + std::to_string(exprIdx);
                                ImVec4 derivCol = item.useCustomColors
                                                      ? ImVec4(item.derivativeColor[0], item.derivativeColor[1], item.derivativeColor[2], 0.85f)
                                                      : (ctrl->toolbarWidget.matchFunctionColor
                                                             ? ImVec4(col.x * 0.75f + 0.25f, col.y * 0.75f + 0.25f, col.z * 0.75f + 0.25f, 0.85f)
                                                             : ImVec4(ctrl->toolbarWidget.derivativeColor[0], ctrl->toolbarWidget.derivativeColor[1], ctrl->toolbarWidget.derivativeColor[2], 0.85f));
                                ImPlot::SetNextLineStyle(derivCol, 1.8f);
                                ImPlot::PlotLine(derivLabel.c_str(), xData.data(), dyData.data(), SAMPLE_COUNT);
                            }

                            // Determine Marker Colors
                            ImVec4 rootCol = item.useCustomColors
                                                 ? ImVec4(item.rootColor[0], item.rootColor[1], item.rootColor[2], 1.0f)
                                                 : (ctrl->toolbarWidget.matchFunctionColor
                                                        ? col
                                                        : ImVec4(ctrl->toolbarWidget.rootColor[0], ctrl->toolbarWidget.rootColor[1], ctrl->toolbarWidget.rootColor[2], 1.0f));

                            ImVec4 extremaCol = item.useCustomColors
                                                    ? ImVec4(item.extremaColor[0], item.extremaColor[1], item.extremaColor[2], 1.0f)
                                                    : (ctrl->toolbarWidget.matchFunctionColor
                                                           ? col
                                                           : ImVec4(ctrl->toolbarWidget.extremaColor[0], ctrl->toolbarWidget.extremaColor[1], ctrl->toolbarWidget.extremaColor[2], 1.0f));

                            ImVec4 tangCol = item.useCustomColors
                                                 ? ImVec4(item.tangentColor[0], item.tangentColor[1], item.tangentColor[2], 1.0f)
                                                 : (ctrl->toolbarWidget.matchFunctionColor
                                                        ? col
                                                        : ImVec4(ctrl->toolbarWidget.tangentColor[0], ctrl->toolbarWidget.tangentColor[1], ctrl->toolbarWidget.tangentColor[2], 1.0f));

                            double scanStep = std::max(0.05, (xMax - xMin) / 100.0);

                            if (ctrl->toolbarWidget.showRoots && item.showRoots)
                            {
                                std::vector<double> rootXs = findRoots(expr, xMin, xMax, scanStep, EPS_ROOT, &context);
                                std::vector<double> rootYs(rootXs.size(), 0.0);
                                if (!rootXs.empty())
                                {
                                    std::string lbl = "f" + std::to_string(exprIdx + 1) + " Roots##" + std::to_string(exprIdx);
                                    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 6.0f, rootCol, 1.0f);
                                    ImPlot::PlotScatter(lbl.c_str(), rootXs.data(), rootYs.data(),
                                                        static_cast<int>(rootXs.size()));
                                }
                            }

                            if (ctrl->toolbarWidget.showExtrema && item.showExtrema)
                            {
                                std::vector<double> extremaXs = findExtrema(expr, xMin, xMax, scanStep, EPS_ROOT, &context);
                                std::vector<double> extremaYs;
                                for (double ex : extremaXs)
                                    extremaYs.push_back(expr.evalWithContext(ex, context));

                                if (!extremaXs.empty())
                                {
                                    std::string lbl = "f" + std::to_string(exprIdx + 1) + " Extrema##" + std::to_string(exprIdx);
                                    ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6.0f, extremaCol, 1.0f);
                                    ImPlot::PlotScatter(lbl.c_str(), extremaXs.data(), extremaYs.data(),
                                                        static_cast<int>(extremaXs.size()));
                                }
                            }

                            if (ctrl->toolbarWidget.showTangent && item.showTangent && ImPlot::IsPlotHovered())
                            {
                                ImPlotPoint mousePt = ImPlot::GetPlotMousePos();
                                double x0 = mousePt.x;
                                double y0 = expr.evalWithContext(x0, context);

                                if (std::isfinite(y0))
                                {
                                    constexpr double h = 1e-5;
                                    double slope = (expr.evalWithContext(x0 + h, context) - expr.evalWithContext(x0 - h, context)) / (2.0 * h);

                                    if (std::isfinite(slope))
                                    {
                                        double tx[2] = {xMin, xMax};
                                        double ty[2] = {y0 + slope * (xMin - x0),
                                                        y0 + slope * (xMax - x0)};

                                        std::string tLineLbl = "f" + std::to_string(exprIdx + 1) + " Tangent Line##Cursor";
                                        ImPlot::SetNextLineStyle(tangCol, 1.8f);
                                        ImPlot::PlotLine(tLineLbl.c_str(), tx, ty, 2);

                                        double px[1] = {x0}, py[1] = {y0};
                                        std::string tPtLbl = "f" + std::to_string(exprIdx + 1) + " Tangent Pt##Cursor";
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond, 8.0f, tangCol, 1.0f);
                                        ImPlot::PlotScatter(tPtLbl.c_str(), px, py, 1);
                                    }
                                }
                            }
                        }

                        if (totalSamples > 0)
                        {
                            context.diagnostics.recordTiming(
                                "Total eval (" + std::to_string(totalSamples) + " pts)", totalEvalMs);
                            if (totalEvalMs > 0.0)
                            {
                                double perPt = totalEvalMs / static_cast<double>(totalSamples) * 1000.0;
                                context.diagnostics.recordTiming("Per-point avg (ns)", perPt);
                            }
                        }
                    }
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }

            // ── Time Domain f(t) ───────────────────────────────────────────
            if (ImGui::BeginTabItem(ICON_FA_CLOCK " Time Domain f(t)"))
            {
                ImPlot::SetNextAxesLimits(xMin, xMax, yMin, yMax, ImPlotCond_Always);
                if (ImPlot::BeginPlot("##TimePlotter", ImVec2(-1, -1), flags))
                {
                    ImPlot::SetupAxes("t (sec)", "Signal f(t)", axisFlags, axisFlags);

                    if (ctrl)
                    {
                        const auto &items = ctrl->exprWidget.getExpressions();
                        constexpr int SAMPLE_COUNT = 600;
                        std::vector<double> tData(SAMPLE_COUNT), yData(SAMPLE_COUNT);
                        for (int i = 0; i < SAMPLE_COUNT; ++i)
                            tData[i] = xMin + (xMax - xMin) * i / static_cast<double>(SAMPLE_COUNT - 1);

                        for (size_t exprIdx = 0; exprIdx < items.size(); ++exprIdx)
                        {
                            const auto &item = items[exprIdx];
                            if (!item.enabled || item.text.empty())
                                continue;

                            try
                            {
                                Expression expr(item.text, context.diagnostics, item.text);
                                if (!expr.isValid())
                                    continue;

                                for (int i = 0; i < SAMPLE_COUNT; ++i)
                                {
                                    context.vars.set("t", MathValue::real(tData[i]));
                                    double y = expr.evalSignalWithContext(tData[i], tData[i], tData[i], context);
                                    yData[i] = std::isfinite(y) ? y : std::numeric_limits<double>::quiet_NaN();
                                }

                                ImVec4 col(item.color[0], item.color[1], item.color[2], 1.0f);
                                std::string lbl = item.text + "##t" + std::to_string(exprIdx);
                                ImPlot::SetNextLineStyle(col, 2.0f);
                                ImPlot::PlotLine(lbl.c_str(), tData.data(), yData.data(), SAMPLE_COUNT);

                                if (ctrl->toolbarWidget.showDerivative && item.showDerivative)
                                {
                                    std::vector<double> dyData(SAMPLE_COUNT);
                                    for (int i = 0; i < SAMPLE_COUNT; ++i)
                                    {
                                        double dy = derivative(expr, tData[i], 1e-5, &context);
                                        dyData[i] = std::isfinite(dy) ? dy : std::numeric_limits<double>::quiet_NaN();
                                    }
                                    std::string derivLabel = "df/dt[" + item.text + "]##DerivT" + std::to_string(exprIdx);
                                    ImVec4 derivCol = item.useCustomColors
                                                          ? ImVec4(item.derivativeColor[0], item.derivativeColor[1], item.derivativeColor[2], 0.85f)
                                                          : (ctrl->toolbarWidget.matchFunctionColor
                                                                 ? ImVec4(col.x * 0.75f + 0.25f, col.y * 0.75f + 0.25f, col.z * 0.75f + 0.25f, 0.85f)
                                                                 : ImVec4(ctrl->toolbarWidget.derivativeColor[0], ctrl->toolbarWidget.derivativeColor[1], ctrl->toolbarWidget.derivativeColor[2], 0.85f));
                                    ImPlot::SetNextLineStyle(derivCol, 1.8f);
                                    ImPlot::PlotLine(derivLabel.c_str(), tData.data(), dyData.data(), SAMPLE_COUNT);
                                }

                                ImVec4 rootCol = item.useCustomColors
                                                     ? ImVec4(item.rootColor[0], item.rootColor[1], item.rootColor[2], 1.0f)
                                                     : (ctrl->toolbarWidget.matchFunctionColor
                                                            ? col
                                                            : ImVec4(ctrl->toolbarWidget.rootColor[0], ctrl->toolbarWidget.rootColor[1], ctrl->toolbarWidget.rootColor[2], 1.0f));

                                ImVec4 extremaCol = item.useCustomColors
                                                        ? ImVec4(item.extremaColor[0], item.extremaColor[1], item.extremaColor[2], 1.0f)
                                                        : (ctrl->toolbarWidget.matchFunctionColor
                                                               ? col
                                                               : ImVec4(ctrl->toolbarWidget.extremaColor[0], ctrl->toolbarWidget.extremaColor[1], ctrl->toolbarWidget.extremaColor[2], 1.0f));

                                if (ctrl->toolbarWidget.showRoots && item.showRoots)
                                {
                                    std::vector<double> rootXs = findRoots(expr, xMin, xMax, 0.1, EPS_ROOT, &context);
                                    std::vector<double> rootYs(rootXs.size(), 0.0);
                                    if (!rootXs.empty())
                                    {
                                        std::string rLbl = "f" + std::to_string(exprIdx + 1) + " Roots##t" + std::to_string(exprIdx);
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 6.0f, rootCol, 1.0f);
                                        ImPlot::PlotScatter(rLbl.c_str(), rootXs.data(), rootYs.data(), static_cast<int>(rootXs.size()));
                                    }
                                }

                                if (ctrl->toolbarWidget.showExtrema && item.showExtrema)
                                {
                                    std::vector<double> extremaXs = findExtrema(expr, xMin, xMax, 0.1, EPS_ROOT, &context);
                                    std::vector<double> extremaYs;
                                    for (double ex : extremaXs)
                                        extremaYs.push_back(expr.evalWithContext(ex, context));
                                    if (!extremaXs.empty())
                                    {
                                        std::string eLbl = "f" + std::to_string(exprIdx + 1) + " Extrema##t" + std::to_string(exprIdx);
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6.0f, extremaCol, 1.0f);
                                        ImPlot::PlotScatter(eLbl.c_str(), extremaXs.data(), extremaYs.data(), static_cast<int>(extremaXs.size()));
                                    }
                                }

                                bool shouldDrawTangent = ctrl->toolbarWidget.showTangent && item.showTangent && ImPlot::IsPlotHovered();
                                if (shouldDrawTangent)
                                {
                                    ImPlotPoint mousePt = ImPlot::GetPlotMousePos();
                                    double t0 = mousePt.x;
                                    double y0 = expr.evalWithContext(t0, context);
                                    if (std::isfinite(y0))
                                    {
                                        constexpr double h = 1e-5;
                                        double slope = (expr.evalWithContext(t0 + h, context) - expr.evalWithContext(t0 - h, context)) / (2.0 * h);
                                        if (std::isfinite(slope))
                                        {
                                            double tx[2] = {xMin, xMax};
                                            double ty[2] = {y0 + slope * (xMin - t0), y0 + slope * (xMax - t0)};
                                            ImVec4 tangCol(ctrl->toolbarWidget.tangentColor[0], ctrl->toolbarWidget.tangentColor[1], ctrl->toolbarWidget.tangentColor[2], 1.0f);
                                            std::string tLbl = "f" + std::to_string(exprIdx + 1) + " Tangent Line##CursorT";
                                            ImPlot::SetNextLineStyle(tangCol, 1.8f);
                                            ImPlot::PlotLine(tLbl.c_str(), tx, ty, 2);
                                        }
                                    }
                                }
                            }
                            catch (...)
                            {
                            }
                        }
                    }
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }

            // ── Discrete f[n] ─────────────────────────────────────────────
            if (ImGui::BeginTabItem(ICON_FA_BARS_STAGGERED " Discrete f[n]"))
            {
                ImPlot::SetNextAxesLimits(xMin, xMax, yMin, yMax, ImPlotCond_Always);
                if (ImPlot::BeginPlot("##DiscretePlotter", ImVec2(-1, -1), flags))
                {
                    ImPlot::SetupAxes("Sample Index n", "f[n]", axisFlags, axisFlags);

                    if (ctrl)
                    {
                        const auto &items = ctrl->exprWidget.getExpressions();
                        int sampleCount = static_cast<int>(std::abs(xMax - xMin)) + 1;
                        if (sampleCount < 10)
                            sampleCount = 10;
                        if (sampleCount > 200)
                            sampleCount = 200;

                        std::vector<double> nData(sampleCount), yData(sampleCount);
                        for (int i = 0; i < sampleCount; ++i)
                            nData[i] = std::floor(xMin) + i;

                        for (size_t exprIdx = 0; exprIdx < items.size(); ++exprIdx)
                        {
                            const auto &item = items[exprIdx];
                            if (!item.enabled || item.text.empty())
                                continue;

                            try
                            {
                                Expression expr(item.text, context.diagnostics, item.text);
                                if (!expr.isValid())
                                    continue;

                                for (int i = 0; i < sampleCount; ++i)
                                {
                                    context.vars.set("n", MathValue::real(nData[i]));
                                    double y = expr.evalSignalWithContext(nData[i], nData[i], nData[i], context);
                                    yData[i] = std::isfinite(y) ? y : 0.0;
                                }

                                ImVec4 col(item.color[0], item.color[1], item.color[2], 1.0f);
                                std::string lbl = item.text + "##n" + std::to_string(exprIdx);
                                ImPlot::SetNextLineStyle(col, 1.5f);
                                ImPlot::PlotStems(lbl.c_str(), nData.data(), yData.data(), sampleCount);

                                if (ctrl->toolbarWidget.showDerivative && item.showDerivative)
                                {
                                    std::vector<double> diffData(sampleCount);
                                    for (int i = 0; i < sampleCount; ++i)
                                    {
                                        double nVal = nData[i];
                                        context.vars.set("n", MathValue::real(nVal));
                                        double yCurr = expr.evalSignalWithContext(nVal, nVal, nVal, context);
                                        context.vars.set("n", MathValue::real(nVal + 1.0));
                                        double yNext = expr.evalSignalWithContext(nVal + 1.0, nVal + 1.0, nVal + 1.0, context);
                                        diffData[i] = (std::isfinite(yCurr) && std::isfinite(yNext)) ? (yNext - yCurr) : 0.0;
                                    }
                                    std::string diffLabel = "Delta f[n] [" + item.text + "]##DiffN" + std::to_string(exprIdx);
                                    ImVec4 diffCol = item.useCustomColors
                                                         ? ImVec4(item.derivativeColor[0], item.derivativeColor[1], item.derivativeColor[2], 0.85f)
                                                         : (ctrl->toolbarWidget.matchFunctionColor
                                                                ? ImVec4(col.x * 0.75f + 0.25f, col.y * 0.75f + 0.25f, col.z * 0.75f + 0.25f, 0.85f)
                                                                : ImVec4(ctrl->toolbarWidget.derivativeColor[0], ctrl->toolbarWidget.derivativeColor[1], ctrl->toolbarWidget.derivativeColor[2], 0.85f));
                                    ImPlot::SetNextLineStyle(diffCol, 1.2f);
                                    ImPlot::PlotStems(diffLabel.c_str(), nData.data(), diffData.data(), sampleCount);
                                }

                                ImVec4 rootCol = item.useCustomColors
                                                     ? ImVec4(item.rootColor[0], item.rootColor[1], item.rootColor[2], 1.0f)
                                                     : (ctrl->toolbarWidget.matchFunctionColor
                                                            ? col
                                                            : ImVec4(ctrl->toolbarWidget.rootColor[0], ctrl->toolbarWidget.rootColor[1], ctrl->toolbarWidget.rootColor[2], 1.0f));

                                ImVec4 extremaCol = item.useCustomColors
                                                        ? ImVec4(item.extremaColor[0], item.extremaColor[1], item.extremaColor[2], 1.0f)
                                                        : (ctrl->toolbarWidget.matchFunctionColor
                                                               ? col
                                                               : ImVec4(ctrl->toolbarWidget.extremaColor[0], ctrl->toolbarWidget.extremaColor[1], ctrl->toolbarWidget.extremaColor[2], 1.0f));

                                ImVec4 tangCol = item.useCustomColors
                                                     ? ImVec4(item.tangentColor[0], item.tangentColor[1], item.tangentColor[2], 1.0f)
                                                     : (ctrl->toolbarWidget.matchFunctionColor
                                                            ? col
                                                            : ImVec4(ctrl->toolbarWidget.tangentColor[0], ctrl->toolbarWidget.tangentColor[1], ctrl->toolbarWidget.tangentColor[2], 1.0f));

                                // ── Discrete Roots (Zero-crossings) ──
                                if (ctrl->toolbarWidget.showRoots && item.showRoots)
                                {
                                    std::vector<double> rXs, rYs;
                                    for (int i = 0; i < sampleCount; ++i)
                                    {
                                        if (std::abs(yData[i]) < 1e-6 || (i > 0 && yData[i] * yData[i - 1] < 0))
                                        {
                                            rXs.push_back(nData[i]);
                                            rYs.push_back(yData[i]);
                                        }
                                    }
                                    if (!rXs.empty())
                                    {
                                        std::string rLbl = "f" + std::to_string(exprIdx + 1) + " Roots##n" + std::to_string(exprIdx);
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 6.0f, rootCol, 1.0f);
                                        ImPlot::PlotScatter(rLbl.c_str(), rXs.data(), rYs.data(), static_cast<int>(rXs.size()));
                                    }
                                }

                                // ── Discrete Extrema (Discrete Local Peaks & Valleys) ──
                                if (ctrl->toolbarWidget.showExtrema && item.showExtrema)
                                {
                                    std::vector<double> eXs, eYs;
                                    for (int i = 1; i < sampleCount - 1; ++i)
                                    {
                                        bool isPeak = (yData[i] > yData[i - 1] && yData[i] > yData[i + 1]);
                                        bool isValley = (yData[i] < yData[i - 1] && yData[i] < yData[i + 1]);
                                        if (isPeak || isValley)
                                        {
                                            eXs.push_back(nData[i]);
                                            eYs.push_back(yData[i]);
                                        }
                                    }
                                    if (!eXs.empty())
                                    {
                                        std::string eLbl = "f" + std::to_string(exprIdx + 1) + " Extrema##n" + std::to_string(exprIdx);
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6.0f, extremaCol, 1.0f);
                                        ImPlot::PlotScatter(eLbl.c_str(), eXs.data(), eYs.data(), static_cast<int>(eXs.size()));
                                    }
                                }

                                // ── Discrete Tangent / Finite Difference Δf[n] ──
                                bool shouldDrawTangent = ctrl->toolbarWidget.showTangent && item.showTangent && ImPlot::IsPlotHovered();
                                if (shouldDrawTangent)
                                {
                                    ImPlotPoint mousePt = ImPlot::GetPlotMousePos();
                                    double n0 = std::round(mousePt.x);
                                    if (n0 >= nData.front() && n0 <= nData.back())
                                    {
                                        context.vars.set("n", MathValue::real(n0));
                                        double y0 = expr.evalSignalWithContext(n0, n0, n0, context);

                                        context.vars.set("n", MathValue::real(n0 - 1.0));
                                        double yPrev = expr.evalSignalWithContext(n0 - 1.0, n0 - 1.0, n0 - 1.0, context);

                                        if (std::isfinite(y0) && std::isfinite(yPrev))
                                        {
                                            double slope = y0 - yPrev; // Discrete finite difference Δy
                                            double tx[2] = {n0 - 1.5, n0 + 1.5};
                                            double ty[2] = {y0 - 1.5 * slope, y0 + 1.5 * slope};

                                            std::string tLbl = "f" + std::to_string(exprIdx + 1) + " Tangent Line##CursorN";
                                            ImPlot::SetNextLineStyle(tangCol, 1.8f);
                                            ImPlot::PlotLine(tLbl.c_str(), tx, ty, 2);

                                            double px[1] = {n0}, py[1] = {y0};
                                            std::string pLbl = "f" + std::to_string(exprIdx + 1) + " Tangent Pt##CursorN";
                                            ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond, 8.0f, tangCol, 1.0f);
                                            ImPlot::PlotScatter(pLbl.c_str(), px, py, 1);
                                        }
                                    }
                                }
                            }
                            catch (...)
                            {
                            }
                        }
                    }
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }

            // ── FFT Spectrogram / Power Spectral Density ─────────────────
            if (ImGui::BeginTabItem(ICON_FA_WAVE_SQUARE " FFT Spectrogram"))
            {
                ImPlot::SetNextAxesLimits(0.0, 50.0, 0.0, 10.0, ImPlotCond_Once);
                if (ImPlot::BeginPlot("##FFTPlotter", ImVec2(-1, -1), flags))
                {
                    ImPlot::SetupAxes("Frequency (Hz)", "Power Spectral Density |X(f)|^2", axisFlags, axisFlags);

                    if (ctrl)
                    {
                        const auto &items = ctrl->exprWidget.getExpressions();
                        constexpr size_t N = 128;
                        double sampleRate = 100.0;
                        std::vector<double> timeSig(N);

                        for (size_t exprIdx = 0; exprIdx < items.size(); ++exprIdx)
                        {
                            const auto &item = items[exprIdx];
                            if (!item.enabled || item.text.empty())
                                continue;
                            try
                            {
                                Expression expr(item.text, context.diagnostics, item.text);
                                if (!expr.isValid())
                                    continue;

                                for (size_t i = 0; i < N; ++i)
                                {
                                    double t = i / sampleRate;
                                    context.vars.set("t", MathValue::real(t));
                                    context.vars.set("x", MathValue::real(t));
                                    double y = expr.evalWithContext(t, context);
                                    timeSig[i] = std::isfinite(y) ? y : 0.0;
                                }

                                std::vector<double> freqs, psd;
                                computePowerSpectrum(timeSig, sampleRate, freqs, psd);

                                std::string lbl = item.text + " (PSD)##fft" + std::to_string(exprIdx);
                                ImVec4 col(item.color[0], item.color[1], item.color[2], 1.0f);
                                ImPlot::SetNextLineStyle(col, 2.0f);
                                ImPlot::PlotLine(lbl.c_str(), freqs.data(), psd.data(), static_cast<int>(freqs.size()));
                            }
                            catch (...)
                            {
                            }
                        }
                    }
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

} // namespace mathstudio::ui
