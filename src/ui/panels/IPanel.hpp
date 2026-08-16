#pragma once

#include <string>
#include "core/EvaluationContext.hpp"

namespace mathstudio::ui
{

    /**
     * @brief Polymorphic interface for all dockable UI panels.
     *
     * Every panel owns its rendering logic and title ID.
     * ImGui context and compiler engine are isolated via EvaluationContext.
     */
    class IPanel
    {
    public:
        virtual ~IPanel() = default;

        /// Returns panel title (e.g. "Control Panel", "Diagnostics Console")
        virtual const char *getTitle() const = 0;

        /// Renders panel content
        virtual void draw(core::EvaluationContext &context) = 0;

        /// Panel open/closed state flag
        bool isOpen = true;
    };

} // namespace mathstudio::ui
