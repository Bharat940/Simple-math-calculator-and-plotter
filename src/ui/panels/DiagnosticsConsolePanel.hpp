#pragma once

#include "ui/panels/IPanel.hpp"
#include "ui/widgets/ConsoleWidget.hpp"
#include "ui/fonts/IconsFontAwesome6.h"

namespace mathstudio::ui
{

    class DiagnosticsConsolePanel : public IPanel
    {
    public:
        const char *getTitle() const override { return ICON_FA_TERMINAL " Diagnostics Console"; }
        void draw(core::EvaluationContext &context) override;

        ConsoleWidget consoleWidget;
    };

} // namespace mathstudio::ui
