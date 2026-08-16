#include "ui/panels/CanvasPanel.hpp"
#include "imgui.h"

namespace mathstudio::ui
{

    CanvasPanel::~CanvasPanel()
    {
        if (m_renderTarget)
        {
            SDL_DestroyTexture(m_renderTarget);
            m_renderTarget = nullptr;
        }
    }

    SDL_Texture *CanvasPanel::getRenderTarget(SDL_Renderer *renderer, int width, int height)
    {
        if (width <= 0 || height <= 0)
            return nullptr;

        if (!m_renderTarget || m_textureWidth != width || m_textureHeight != height)
        {
            if (m_renderTarget)
            {
                SDL_DestroyTexture(m_renderTarget);
            }
            m_renderTarget = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                width, height);
            m_textureWidth = width;
            m_textureHeight = height;
        }
        return m_renderTarget;
    }

    void CanvasPanel::draw(core::EvaluationContext &)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin(getTitle(), &isOpen))
        {
            ImVec2 viewportSize = ImGui::GetContentRegionAvail();

            if (m_renderTarget && viewportSize.x > 0 && viewportSize.y > 0)
            {
                ImGui::Image(
                    reinterpret_cast<ImTextureID>(m_renderTarget),
                    viewportSize);
            }
            else
            {
                ImGui::TextDisabled("Rendering offscreen plot target...");
            }

            // Viewport interaction handling (Middle mouse drag, scroll zoom)
            if (ImGui::IsWindowHovered())
            {
                ImGuiIO &io = ImGui::GetIO();
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                {
                    // Pan action
                }
                if (io.MouseWheel != 0.0f)
                {
                    // Zoom action
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

} // namespace mathstudio::ui
