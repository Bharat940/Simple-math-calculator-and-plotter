#pragma once

#include "core/EvaluationContext.hpp"

namespace mathstudio::ui
{

    /**
     * @brief Base widget interface for reusable UI components.
     */
    class IWidget
    {
    public:
        virtual ~IWidget() = default;

        /// Renders widget UI
        virtual void render(core::EvaluationContext &context) = 0;
    };

} // namespace mathstudio::ui
