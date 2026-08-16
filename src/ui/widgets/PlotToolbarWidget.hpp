#pragma once

#include "ui/widgets/IWidget.hpp"

namespace mathstudio::ui
{

    class PlotToolbarWidget : public IWidget
    {
    public:
        void render(core::EvaluationContext &context) override;

        // Global Master Overlay Toggles
        bool showGrid = true;
        bool showRoots = true;
        bool showExtrema = true;
        bool showTangent = true;
        bool showDerivative = false;
        bool showLabels = true;

        // Global Marker Color Customization
        bool matchFunctionColor = true;
        float rootColor[3] = {0.20f, 0.90f, 0.30f};
        float extremaColor[3] = {0.90f, 0.30f, 0.20f};
        float tangentColor[3] = {1.00f, 0.80f, 0.20f};
        float derivativeColor[3] = {0.80f, 0.40f, 1.00f};
    };

} // namespace mathstudio::ui
