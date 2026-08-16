#pragma once

#include "ui/widgets/IWidget.hpp"

namespace mathstudio::ui
{

    class StatusBarWidget : public IWidget
    {
    public:
        void render(core::EvaluationContext &context) override;

        double mouseX = 0.0;
        double mouseY = 0.0;
    };

} // namespace mathstudio::ui
