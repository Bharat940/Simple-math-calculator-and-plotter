#pragma once

#include "ui/widgets/IWidget.hpp"

namespace mathstudio::ui
{

    class AxisEditorWidget : public IWidget
    {
    public:
        void render(core::EvaluationContext &context) override;

        float xMin = -10.0f;
        float xMax = 10.0f;
        float yMin = -6.0f;
        float yMax = 6.0f;
        int gridMode = 0; // 0: Auto, 1: Fixed, 2: Loose, 3: Dense
        float discThreshold = 10000.0f;
    };

} // namespace mathstudio::ui
