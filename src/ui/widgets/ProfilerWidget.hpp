#pragma once

#include "ui/widgets/IWidget.hpp"

namespace mathstudio::ui
{

    class ProfilerWidget : public IWidget
    {
    public:
        void render(core::EvaluationContext &context) override;
    };

} // namespace mathstudio::ui
