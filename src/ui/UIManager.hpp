#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include "ui/panels/IPanel.hpp"
#include "core/EvaluationContext.hpp"

namespace mathstudio::ui
{

    class UIManager
    {
    public:
        static UIManager &instance();

        bool init(SDL_Window *window, SDL_Renderer *renderer);
        void processEvent(const SDL_Event *event);
        void beginFrame();
        void setupDockspace();
        void render(core::EvaluationContext &context);
        void shutdown();

        template <typename T, typename... Args>
        T *registerPanel(Args &&...args)
        {
            auto panel = std::make_unique<T>(std::forward<Args>(args)...);
            T *ptr = panel.get();
            m_panels.push_back(std::move(panel));
            return ptr;
        }

        template <typename T>
        T *getPanel()
        {
            for (auto &panel : m_panels)
            {
                if (auto p = dynamic_cast<T *>(panel.get()))
                {
                    return p;
                }
            }
            return nullptr;
        }

        void clearPanels() { m_panels.clear(); }
        const std::vector<std::unique_ptr<IPanel>> &getPanels() const { return m_panels; }

        bool isLayoutLocked() const { return m_lockLayout; }
        void setLockLayout(bool lock) { m_lockLayout = lock; }

    private:
        UIManager() = default;

        SDL_Window *m_window = nullptr;
        SDL_Renderer *m_renderer = nullptr;
        bool m_initialized = false;
        bool m_lockLayout = true; // Default: Fixed panels, no accidental drag/docking!
        std::vector<std::unique_ptr<IPanel>> m_panels;
    };

} // namespace mathstudio::ui
