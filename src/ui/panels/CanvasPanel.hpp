#pragma once

#include "ui/panels/IPanel.hpp"
#include <SDL2/SDL.h>

namespace mathstudio::ui
{

    class CanvasPanel : public IPanel
    {
    public:
        CanvasPanel() { isOpen = false; }
        ~CanvasPanel() override;

        const char *getTitle() const override { return "2D Plot Canvas"; }
        void draw(core::EvaluationContext &context) override;

        SDL_Texture *getRenderTarget(SDL_Renderer *renderer, int width, int height);

    private:
        SDL_Texture *m_renderTarget = nullptr;
        int m_textureWidth = 0;
        int m_textureHeight = 0;
    };

} // namespace mathstudio::ui
