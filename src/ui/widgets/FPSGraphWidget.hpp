#pragma once

#include "ui/widgets/IWidget.hpp"

namespace mathstudio::ui
{
    class FPSGraphWidget : public IWidget
    {
    public:
        FPSGraphWidget() = default;

        void render(core::EvaluationContext &context) override;
    };

} // namespace mathstudio::ui
