#include "ui/widgets/ExpressionEditorWidget.hpp"
#include "ui/UIManager.hpp"
#include "ui/panels/VariableInspectorPanel.hpp"
#include "math/tokenizer.h"
#include "imgui.h"
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <vector>

namespace mathstudio::ui
{

    ExpressionEditorWidget::ExpressionEditorWidget()
    {
        // Default starting expressions: f1 has overlays ON, f2 has overlays OFF by default (Industry Standard)
        addExpression("sin(x)");
        addExpression("cos(x)");
    }

    void ExpressionEditorWidget::addExpression(const std::string &expr)
    {
        ExpressionItem item;
        item.text = expr;

        // Industry Standard Rule: Only the first function (f1) has analysis overlays ON by default.
        // Newly added functions (f2, f3, etc.) start with overlays OFF to prevent plot clutter.
        bool isFirst = m_expressions.empty();
        item.showRoots = isFirst;
        item.showExtrema = isFirst;
        item.showTangent = isFirst;

        // Distinct vibrant color palette for line colors
        static const float defaultColors[][3] = {
            {0.00f, 0.67f, 1.00f}, // f1: Electric Blue
            {1.00f, 0.40f, 0.31f}, // f2: Coral Red
            {0.31f, 0.89f, 0.55f}, // f3: Emerald Green
            {1.00f, 0.78f, 0.20f}, // f4: Vivid Gold
            {0.75f, 0.40f, 0.89f}  // f5: Electric Purple
        };

        // Distinct custom marker palettes
        static const float defaultRootColors[][3] = {
            {0.20f, 0.95f, 0.40f}, // f1: Lime Green
            {0.00f, 0.90f, 0.90f}, // f2: Bright Cyan
            {1.00f, 0.85f, 0.20f}, // f3: Yellow
            {1.00f, 0.30f, 0.80f}, // f4: Magenta
            {0.40f, 0.80f, 1.00f}  // f5: Sky Blue
        };

        static const float defaultExtremaColors[][3] = {
            {1.00f, 0.25f, 0.25f}, // f1: Red
            {1.00f, 0.50f, 0.10f}, // f2: Orange
            {0.90f, 0.20f, 0.90f}, // f3: Pink/Purple
            {0.20f, 0.80f, 1.00f}, // f4: Ice Blue
            {0.40f, 0.95f, 0.30f}  // f5: Spring Green
        };

        size_t idx = m_expressions.size() % 5;
        item.color[0] = defaultColors[idx][0];
        item.color[1] = defaultColors[idx][1];
        item.color[2] = defaultColors[idx][2];

        item.rootColor[0] = defaultRootColors[idx][0];
        item.rootColor[1] = defaultRootColors[idx][1];
        item.rootColor[2] = defaultRootColors[idx][2];

        item.extremaColor[0] = defaultExtremaColors[idx][0];
        item.extremaColor[1] = defaultExtremaColors[idx][1];
        item.extremaColor[2] = defaultExtremaColors[idx][2];

        // Tangent line matches curve color by default
        item.tangentColor[0] = defaultColors[idx][0];
        item.tangentColor[1] = defaultColors[idx][1];
        item.tangentColor[2] = defaultColors[idx][2];

        m_expressions.push_back(item);
    }

    void ExpressionEditorWidget::render(core::EvaluationContext &)
    {
        ImGui::TextDisabled("ACTIVE EXPRESSIONS (DRAG TO REORDER Z-INDEX)");
        ImGui::Separator();
        ImGui::Spacing();

        for (size_t i = 0; i < m_expressions.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));

            char buf[256];
            std::strncpy(buf, m_expressions[i].text.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';

            // Drag & Drop layer ordering handle (Industry Standard)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.22f, 0.28f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.4f, 0.55f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
            ImGui::Button(ICON_FA_GRIP_VERTICAL "##drag", ImVec2(24.0f, 24.0f));
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Click & Drag to Reorder Layer Z-Index");

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                size_t payloadIdx = i;
                ImGui::SetDragDropPayload("EXPR_REORDER_ITEM", &payloadIdx, sizeof(size_t));
                ImGui::Text("Move %s", buf);
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("EXPR_REORDER_ITEM"))
                {
                    size_t sourceIdx = *static_cast<const size_t *>(payload->Data);
                    if (sourceIdx != i && sourceIdx < m_expressions.size())
                    {
                        std::swap(m_expressions[sourceIdx], m_expressions[i]);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // Precision Layer Shifting Buttons (Up / Down)
            ImGui::SameLine();
            if (i > 0)
            {
                std::string upLbl = ICON_FA_ANGLE_UP "##up" + std::to_string(i);
                if (ImGui::SmallButton(upLbl.c_str()))
                {
                    std::swap(m_expressions[i], m_expressions[i - 1]);
                    ImGui::PopID();
                    break;
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Move Up (Bring Forward)");
                ImGui::SameLine();
            }

            if (i + 1 < m_expressions.size())
            {
                std::string dnLbl = ICON_FA_ANGLE_DOWN "##dn" + std::to_string(i);
                if (ImGui::SmallButton(dnLbl.c_str()))
                {
                    std::swap(m_expressions[i], m_expressions[i + 1]);
                    ImGui::PopID();
                    break;
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Move Down (Send Backward)");
                ImGui::SameLine();
            }

            ImGui::Checkbox("##Enable", &m_expressions[i].enabled);
            ImGui::SameLine();
            ImGui::ColorEdit3("##Color", m_expressions[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            float availW = ImGui::GetContentRegionAvail().x;
            bool showRemove = (m_expressions.size() > 1);
            float reservedW = showRemove ? 38.0f : 5.0f;
            float inputW = std::max(60.0f, availW - reservedW);

            ImGui::SetNextItemWidth(inputW);
            if (ImGui::InputText("##ExprInput", buf, sizeof(buf)))
            {
                m_expressions[i].text = buf;
            }

            if (showRemove)
            {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_TRASH))
                {
                    m_expressions.erase(m_expressions.begin() + i);
                    ImGui::PopID();
                    break;
                }
            }

            // Industry Standard Quick-Add Badge (Desmos): Prompt user to initialize unadded parameters
            VariableInspectorPanel *varPanel = UIManager::instance().getPanel<VariableInspectorPanel>();
            static const std::unordered_set<std::string> SYSTEM_SIGNALS = {"x", "t", "n", "theta", "z", "ans"};
            std::vector<std::string> unaddedParams;

            if (!m_expressions[i].text.empty())
            {
                try
                {
                    auto tokens = tokenize(m_expressions[i].text);
                    for (const auto &tok : tokens)
                    {
                        if (tok.type == TokenType::Variable && !SYSTEM_SIGNALS.count(tok.value))
                        {
                            if (varPanel && !varPanel->tableWidget.hasParameter(tok.value))
                            {
                                if (std::find(unaddedParams.begin(), unaddedParams.end(), tok.value) == unaddedParams.end())
                                {
                                    unaddedParams.push_back(tok.value);
                                }
                            }
                        }
                    }
                }
                catch (...)
                {
                }
            }

            if (!unaddedParams.empty())
            {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.7f, 0.2f, 1.0f), "Add sliders:");
                ImGui::SameLine();
                for (const auto &pName : unaddedParams)
                {
                    std::string btnLbl = "+ " + pName + "##p" + std::to_string(i);
                    if (ImGui::SmallButton(btnLbl.c_str()))
                    {
                        if (varPanel)
                            varPanel->tableWidget.addParameter(pName, 1.0);
                    }
                    ImGui::SameLine();
                }
                std::string allLbl = "+ Add All##p" + std::to_string(i);
                if (ImGui::SmallButton(allLbl.c_str()))
                {
                    if (varPanel)
                    {
                        for (const auto &pName : unaddedParams)
                        {
                            varPanel->tableWidget.addParameter(pName, 1.0);
                        }
                    }
                }
            }

            // Expandable settings per function
            std::string settingsNodeLabel = "Per-Function Options (f" + std::to_string(i + 1) + ")";
            if (ImGui::TreeNode(settingsNodeLabel.c_str()))
            {
                ImGui::Checkbox("Analyze Roots", &m_expressions[i].showRoots);
                ImGui::SameLine();
                ImGui::Checkbox("Analyze Extrema", &m_expressions[i].showExtrema);

                ImGui::Checkbox("Show Tangent", &m_expressions[i].showTangent);
                ImGui::SameLine();
                ImGui::Checkbox("Show Derivative", &m_expressions[i].showDerivative);

                if (ImGui::TreeNode("Custom Marker Colors"))
                {
                    ImGui::Checkbox("Use Custom Colors", &m_expressions[i].useCustomColors);
                    if (m_expressions[i].useCustomColors)
                    {
                        ImGui::ColorEdit3("Root Marker Color", m_expressions[i].rootColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit3("Extrema Marker Color", m_expressions[i].extremaColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit3("Tangent Line Color", m_expressions[i].tangentColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit3("Derivative Curve Color", m_expressions[i].derivativeColor, ImGuiColorEditFlags_NoInputs);
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            ImGui::Spacing();
            ImGui::PopID();
        }

        ImGui::Spacing();
        float availW = ImGui::GetContentRegionAvail().x;
        float addBtnW = 75.0f;
        float newExprInputW = std::max(80.0f, availW - addBtnW - 10.0f);

        ImGui::SetNextItemWidth(newExprInputW);
        bool enterPressed = ImGui::InputText("##NewExpr", m_newExprBuffer, sizeof(m_newExprBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if ((ImGui::Button(ICON_FA_PLUS " Add", ImVec2(addBtnW, 0)) || enterPressed) && m_newExprBuffer[0] != '\0')
        {
            addExpression(m_newExprBuffer);
            m_newExprBuffer[0] = '\0';
        }
    }

} // namespace mathstudio::ui
