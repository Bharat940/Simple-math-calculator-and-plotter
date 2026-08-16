#pragma once

#include "ui/panels/IPanel.hpp"
#include "ui/fonts/IconsFontAwesome6.h"

namespace mathstudio::ui
{

    class PlotPanel : public IPanel
    {
    public:
        const char *getTitle() const override { return ICON_FA_CHART_LINE " Scientific Plotter"; }
        void draw(core::EvaluationContext &context) override;
    };

} // namespace mathstudio::ui
