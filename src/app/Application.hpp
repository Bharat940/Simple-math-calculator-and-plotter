#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include "cli/CliParser.hpp"
#include "math/expression.h"
#include "core/EvaluationContext.hpp"
#include "ui/UIManager.hpp"
#include <vector>
#include <string>

namespace mathstudio::app
{

    class Application
    {
    public:
        explicit Application(const cli::CliConfig &config, std::string initialExprStr = "x^2");
        ~Application();

        bool init();
        int run();

    private:
        void processEvents(SDL_Event &event);
        void shutdown();

        cli::CliConfig m_config;
        std::string m_initialExprStr;

        SDL_Window *m_window = nullptr;
        SDL_Renderer *m_renderer = nullptr;
        TTF_Font *m_font = nullptr;

        std::vector<Expression> m_expressions;
        std::vector<std::string> m_expressionLabels;

        double m_xmin = -10.0, m_xmax = 10.0;
        double m_ymin = -10.0, m_ymax = 10.0;

        bool m_running = true;
        bool m_needsRedraw = true;
        bool m_rootsDirty = false;
        bool m_extremaDirty = true;
        bool m_showTangent = false;
        bool m_showGrid = true;
        bool m_showExtrema = false;
        bool m_showRoots = false;
        size_t m_activeExpr = 0;
        int m_mouseX = 0, m_mouseY = 0;
    };

} // namespace mathstudio::app
