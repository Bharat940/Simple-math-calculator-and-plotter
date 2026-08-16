#pragma once

#include "ui/panels/IPanel.hpp"
#include "ui/widgets/ProfilerWidget.hpp"
#include "ui/widgets/FPSGraphWidget.hpp"
#include "ui/fonts/IconsFontAwesome6.h"

namespace mathstudio::ui
{

    class PerformanceProfilerPanel : public IPanel
    {
    public:
        const char *getTitle() const override { return ICON_FA_GAUGE " Performance Profiler"; }
        void draw(core::EvaluationContext &context) override;

        ProfilerWidget profilerWidget;
        FPSGraphWidget fpsWidget;
    };

} // namespace mathstudio::ui
