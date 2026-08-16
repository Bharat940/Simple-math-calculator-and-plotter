#pragma once

#include "ui/panels/IPanel.hpp"
#include "ui/widgets/ExpressionEditorWidget.hpp"
#include "ui/widgets/PlotToolbarWidget.hpp"
#include "ui/widgets/AxisEditorWidget.hpp"
#include "ui/fonts/IconsFontAwesome6.h"

namespace mathstudio::ui
{

    class ControlPanel : public IPanel
    {
    public:
        ControlPanel() = default;

        const char *getTitle() const override { return ICON_FA_SLIDERS " Control Panel"; }
        void draw(core::EvaluationContext &context) override;

        ExpressionEditorWidget exprWidget;
        PlotToolbarWidget toolbarWidget;
        AxisEditorWidget axisWidget;
    };

} // namespace mathstudio::ui
