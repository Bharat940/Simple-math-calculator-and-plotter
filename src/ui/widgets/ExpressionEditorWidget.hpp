#pragma once

#include "ui/widgets/IWidget.hpp"
#include <vector>
#include <string>

namespace mathstudio::ui
{

    struct ExpressionItem
    {
        std::string text;
        bool enabled = true;
        float color[3] = {0.31f, 0.64f, 0.89f};

        // Per-Function Analysis & Overlay Toggles
        bool showRoots = true;
        bool showExtrema = true;
        bool showTangent = true;
        bool showDerivative = true;

        // Per-Function Marker Color Customization
        bool useCustomColors = false;
        float rootColor[3] = {0.20f, 0.90f, 0.30f};
        float extremaColor[3] = {0.90f, 0.30f, 0.20f};
        float tangentColor[3] = {1.00f, 0.80f, 0.20f};
        float derivativeColor[3] = {0.80f, 0.40f, 1.00f};
    };

    class ExpressionEditorWidget : public IWidget
    {
    public:
        ExpressionEditorWidget();

        void render(core::EvaluationContext &context) override;

        const std::vector<ExpressionItem> &getExpressions() const { return m_expressions; }
        std::vector<ExpressionItem> &getExpressions() { return m_expressions; }
        void addExpression(const std::string &expr);
        void clear() { m_expressions.clear(); }

    private:
        std::vector<ExpressionItem> m_expressions;
        char m_newExprBuffer[256] = {0};
    };

} // namespace mathstudio::ui
