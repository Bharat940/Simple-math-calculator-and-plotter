#pragma once

#include <vector>
#include <string>
#include <functional>
#include "core/EvaluationContext.hpp"

namespace mathstudio::ui
{

    struct CommandAction
    {
        std::string name;
        std::string category;
        std::string shortcut;
        std::function<void(core::EvaluationContext &)> handler;
    };

    class CommandPalette
    {
    public:
        static CommandPalette &instance();

        void registerCommand(const CommandAction &action);
        void open();
        void close();
        bool isOpen() const { return m_isOpen; }

        void render(core::EvaluationContext &context);

    private:
        CommandPalette();
        void populateDefaultCommands();

        bool m_isOpen = false;
        char m_searchQuery[128] = {0};
        std::vector<CommandAction> m_actions;
    };

} // namespace mathstudio::ui
