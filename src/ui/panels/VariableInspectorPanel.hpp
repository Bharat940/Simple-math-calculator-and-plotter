#pragma once

#include "ui/panels/IPanel.hpp"
#include "ui/widgets/VariableTableWidget.hpp"
#include "ui/fonts/IconsFontAwesome6.h"

namespace mathstudio::ui
{

    class VariableInspectorPanel : public IPanel
    {
    public:
        const char *getTitle() const override { return ICON_FA_TABLE " Variable Inspector"; }
        void draw(core::EvaluationContext &context) override;

        VariableTableWidget tableWidget;
    };

} // namespace mathstudio::ui
