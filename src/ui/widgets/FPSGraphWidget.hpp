#pragma once

#include "ui/widgets/IWidget.hpp"
#include <vector>

namespace mathstudio::ui
{

    class FPSGraphWidget : public IWidget
    {
    public:
        FPSGraphWidget();

        void render(core::EvaluationContext &context) override;

    private:
        std::vector<float> m_frameTimeHistory;
        size_t m_maxHistory = 100;
    };

} // namespace mathstudio::ui
