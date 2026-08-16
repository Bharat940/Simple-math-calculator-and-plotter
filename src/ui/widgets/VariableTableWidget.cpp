#include "ui/widgets/VariableTableWidget.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/UIManager.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "math/tokenizer.h"
#include "math/expression.h"
#include "imgui.h"
#include <cmath>
#include <cstring>
#include <algorithm>
#include <unordered_set>

namespace mathstudio::ui
{

    VariableTableWidget::VariableTableWidget()
    {
    }

    void VariableTableWidget::addParameter(const std::string &name, double defaultVal)
    {
        m_manualParams.insert(name);
        if (m_userParams.find(name) == m_userParams.end())
        {
            ParamConfig cfg;
            cfg.value = defaultVal;
            m_userParams[name] = cfg;
        }
    }

    bool VariableTableWidget::hasParameter(const std::string &name) const
    {
        return m_userParams.find(name) != m_userParams.end();
    }

    void VariableTableWidget::render(core::EvaluationContext &context)
    {
        ImGui::TextDisabled("LIVE VARIABLE INSPECTOR & PARAMETERS");
        ImGui::Separator();
        ImGui::Spacing();

        // ── 1. Dynamic Parameter Auto-Discovery ─────────────────────────────
        ControlPanel *ctrl = UIManager::instance().getPanel<ControlPanel>();
        static const std::unordered_set<std::string> SYSTEM_SIGNALS = {"x", "ans"};
        std::unordered_set<std::string> activeDiscoveredParams;

        if (ctrl)
        {
            const auto &items = ctrl->exprWidget.getExpressions();
            for (const auto &item : items)
            {
                if (!item.enabled || item.text.empty())
                    continue;

                try
                {
                    bool isExprValid = false;
                    try
                    {
                        Expression checkExpr(item.text);
                        isExprValid = checkExpr.isValid();
                    }
                    catch (...)
                    {
                    }

                    auto tokens = tokenize(item.text);
                    static const std::unordered_set<std::string> FN_PREFIXES = {
                        "c", "co", "s", "si", "l", "lo", "t", "ta", "sq", "sqr", "a", "as", "ac", "at", "e", "ex", "f", "fl", "m", "ma", "mi", "p", "po"};

                    for (const auto &tok : tokens)
                    {
                        if (tok.type == TokenType::Variable && !SYSTEM_SIGNALS.count(tok.value))
                        {
                            // Avoid registering partial function name prefixes while typing incomplete expressions
                            if (!isExprValid && FN_PREFIXES.count(tok.value))
                                continue;

                            activeDiscoveredParams.insert(tok.value);
                            // Auto-initialize newly discovered parameter from context.vars (if CLI provided) or default 1.0
                            if (m_userParams.find(tok.value) == m_userParams.end())
                            {
                                ParamConfig cfg;
                                if (context.vars.has(tok.value))
                                {
                                    MathValue mv = context.vars.get(tok.value);
                                    cfg.value = mv.isReal() ? mv.getReal() : 1.0;
                                }
                                else
                                {
                                    cfg.value = 1.0;
                                }

                                constexpr float MAX_SAFE_BOUND = 10000.0f;
                                if (!std::isfinite(cfg.value))
                                {
                                    cfg.value = 1.0;
                                }
                                cfg.value = std::clamp(cfg.value, -10000.0, 10000.0);

                                if (cfg.value < cfg.minVal)
                                {
                                    cfg.minVal = (std::max)(-MAX_SAFE_BOUND, static_cast<float>(std::floor(cfg.value - std::abs(cfg.value) * 0.5)));
                                }
                                if (cfg.value > cfg.maxVal)
                                {
                                    cfg.maxVal = (std::min)(MAX_SAFE_BOUND, static_cast<float>(std::ceil(cfg.value + std::abs(cfg.value) * 0.5)));
                                }
                                m_userParams[tok.value] = cfg;
                            }
                        }
                    }
                }
                catch (...)
                {
                }
            }

            // Auto-prune stale / unreferenced parameters (unless manually added by user)
            for (auto it = m_userParams.begin(); it != m_userParams.end();)
            {
                if (activeDiscoveredParams.find(it->first) == activeDiscoveredParams.end() &&
                    m_manualParams.find(it->first) == m_manualParams.end())
                {
                    context.vars.remove(it->first);
                    it = m_userParams.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // ── 2. Frame-by-Frame Live Animation Update ──────────────────────────
        float dt = ImGui::GetIO().DeltaTime;
        for (auto &[paramName, p] : m_userParams)
        {
            if (p.animating && dt > 0.0f)
            {
                float delta = p.animSpeed * dt * (p.animForward ? 1.0f : -1.0f);
                float newVal = static_cast<float>(p.value) + delta;

                if (newVal >= p.maxVal)
                {
                    newVal = p.maxVal;
                    p.animForward = false;
                }
                else if (newVal <= p.minVal)
                {
                    newVal = p.minVal;
                    p.animForward = true;
                }

                p.value = static_cast<double>(newVal);
            }
            context.vars.set(paramName, MathValue::real(p.value));
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit;

        std::string toDeleteParam;
        // 4 Columns: Name, Status, Play & Slider, Options (↺, ⚙, 🗑)
        if (ImGui::BeginTable("VariableTable", 4, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 55.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Play & Slider", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Opt", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableHeadersRow();

            // 1. Constants Section
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled(ICON_FA_BOOK " Constants");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("System");
            ImGui::TableSetColumnIndex(2);
            ImGui::TextDisabled("Read-Only");
            ImGui::TableSetColumnIndex(3);
            ImGui::TextDisabled("-");

            static const struct
            {
                const char *name;
                double val;
            } constants[] = {
                {"pi", 3.141592653589793},
                {"e", 2.718281828459045},
                {"tau", 6.283185307179586},
                {"phi", 1.618033988749895}};

            for (const auto &c : constants)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", c.name);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Real");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.6f", c.val);
                ImGui::TableSetColumnIndex(3);
                ImGui::TextDisabled("-");
            }

            // 2. System Signals Section
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled(ICON_FA_WAVE_SQUARE " Signals");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("System");
            ImGui::TableSetColumnIndex(2);
            ImGui::TextDisabled("Plot Dynamic");
            ImGui::TableSetColumnIndex(3);
            ImGui::TextDisabled("-");

            static const char *signals[] = {"x", "t", "n", "theta", "z", "ans"};
            for (const auto *sig : signals)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", sig);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Signal");
                ImGui::TableSetColumnIndex(2);
                double val = context.vars.get(sig).getReal();
                ImGui::Text("%.4f", val);
                ImGui::TableSetColumnIndex(3);
                ImGui::TextDisabled("-");
            }

            // 3. Dynamic User Parameters & Animation Controls Section
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled(ICON_FA_SLIDERS " Parameters");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("Status");
            ImGui::TableSetColumnIndex(2);
            ImGui::TextDisabled("Play / Animation Sliders");
            ImGui::TableSetColumnIndex(3);
            ImGui::TextDisabled("Set");

            for (auto &[paramName, p] : m_userParams)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", paramName.c_str());
                ImGui::TableSetColumnIndex(1);

                if (activeDiscoveredParams.count(paramName))
                {
                    ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), "Active");
                }
                else
                {
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Manual");
                }

                // Column 2: Play/Pause Button + Slider
                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(paramName.c_str());

                const char *playLabel = p.animating ? ICON_FA_PAUSE " PAUSE" : ICON_FA_PLAY " PLAY";
                // Warm Amber (Media Pause) when animating; Soft Green when stopped
                ImVec4 btnCol = p.animating ? ImVec4(0.9f, 0.55f, 0.1f, 1.0f) : ImVec4(0.2f, 0.7f, 0.3f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Button, btnCol);
                if (ImGui::Button(playLabel, ImVec2(75.0f, 0.0f)))
                {
                    p.animating = !p.animating;
                }
                ImGui::PopStyleColor();

                ImGui::SameLine();

                float floatVal = static_cast<float>(p.value);
                ImGui::SetNextItemWidth(-1.0f); // Stretch slider full width of remaining column space
                if (ImGui::SliderFloat("##Slider", &floatVal, p.minVal, p.maxVal, "%.3f"))
                {
                    p.value = static_cast<double>(floatVal);
                    context.vars.set(paramName, MathValue::real(p.value));
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Drag slider or Ctrl+Click / Double-Click to type exact value!");
                }

                // Column 3: Dedicated Reset to 1.0 & Gear Settings Buttons
                ImGui::TableSetColumnIndex(3);

                // Quick Reset to 1.0 Button
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.25f, 0.32f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.7f, 1.0f));
                if (ImGui::Button(ICON_FA_ROTATE_LEFT, ImVec2(24.0f, 0.0f)))
                {
                    p.value = 1.0;
                    context.vars.set(paramName, MathValue::real(1.0));
                }
                ImGui::PopStyleColor(2);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Reset %s value to 1.0", paramName.c_str());

                ImGui::SameLine();

                // Gear Settings Button
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.26f, 0.32f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.42f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.38f, 0.65f, 1.0f));

                if (ImGui::Button(ICON_FA_GEAR, ImVec2(24.0f, 0.0f)))
                {
                    ImGui::OpenPopup("ParamConfigPopup");
                }
                ImGui::PopStyleColor(3);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Configure Bounds & Exact Value");

                ImGui::SameLine();

                // Trash / Delete Parameter Button
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.18f, 0.18f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.25f, 0.25f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
                if (ImGui::Button(ICON_FA_TRASH_CAN, ImVec2(24.0f, 0.0f)))
                {
                    toDeleteParam = paramName;
                }
                ImGui::PopStyleColor(3);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Delete Parameter Variable '%s'", paramName.c_str());

                if (ImGui::BeginPopup("ParamConfigPopup"))
                {
                    ImGui::Text("Parameter: %s", paramName.c_str());
                    ImGui::Separator();

                    double exactVal = p.value;
                    if (ImGui::InputDouble("Exact Value", &exactVal, 0.1, 1.0, "%.4f"))
                    {
                        exactVal = std::clamp(exactVal, static_cast<double>(p.minVal), static_cast<double>(p.maxVal));
                        p.value = exactVal;
                        context.vars.set(paramName, MathValue::real(p.value));
                    }

                    if (ImGui::Button(ICON_FA_ROTATE_LEFT " Reset Value to 1.0"))
                    {
                        p.value = 1.0;
                        context.vars.set(paramName, MathValue::real(1.0));
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_TRASH_CAN " Delete Parameter"))
                    {
                        toDeleteParam = paramName;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::Spacing();
                    ImGui::DragFloat("Min Bound", &p.minVal, 0.1f, -100.0f, p.maxVal - 0.1f);
                    ImGui::DragFloat("Max Bound", &p.maxVal, 0.1f, p.minVal + 0.1f, 100.0f);
                    ImGui::DragFloat("Anim Speed", &p.animSpeed, 0.1f, 0.1f, 20.0f);

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        if (!toDeleteParam.empty())
        {
            context.vars.remove(toDeleteParam);
            m_manualParams.erase(toDeleteParam);
            m_userParams.erase(toDeleteParam);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Dynamic Parameter Manual Addition
        static char newVarBuf[64] = "";
        float availW = ImGui::GetContentRegionAvail().x;
        float addBtnW = 85.0f;
        float inputW = std::max(80.0f, availW - addBtnW - 10.0f);

        ImGui::SetNextItemWidth(inputW);
        bool enterPressed = ImGui::InputText("##NewParam", newVarBuf, sizeof(newVarBuf), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if ((ImGui::Button(ICON_FA_PLUS " Param", ImVec2(addBtnW, 0)) || enterPressed) && newVarBuf[0] != '\0')
        {
            addParameter(newVarBuf, 1.0);
            context.vars.set(newVarBuf, MathValue::real(1.0));
            newVarBuf[0] = '\0';
        }
    }

} // namespace mathstudio::ui
