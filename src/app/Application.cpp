#include "app/Application.hpp"
#include "cli/CliRunner.hpp"
#include "ui/panels/ControlPanel.hpp"
#include "ui/panels/CanvasPanel.hpp"
#include "ui/panels/VariableInspectorPanel.hpp"
#include "ui/panels/PerformanceProfilerPanel.hpp"
#include "ui/panels/DiagnosticsConsolePanel.hpp"
#include "ui/panels/PlotPanel.hpp"
#include "math/numerical.h"
#include "math/solver.h"
#include "imgui.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#include <SDL_syswm.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

namespace mathstudio::app
{

    constexpr double Y_AUTO_CLAMP = 10.0;

    Application::Application(const cli::CliConfig &config, std::string initialExprStr)
        : m_config(config), m_initialExprStr(std::move(initialExprStr))
    {
    }

    Application::~Application()
    {
        shutdown();
    }

    bool Application::init()
    {
        // 1. Parse Initial Expressions for GUI Plotting
        try
        {
            auto parts = cli::CliParser::splitExpressions(m_initialExprStr);
            for (const auto &p : parts)
            {
                if (!cli::CliParser::validateExpression(p))
                {
                    cli::CliParser::cliError("invalid expression syntax: " + p, 2);
                    return false;
                }
                m_expressions.emplace_back(p);
                m_expressionLabels.push_back(p);
            }
        }
        catch (const std::exception &ex)
        {
            cli::CliParser::cliError(ex.what(), 2);
            return false;
        }

        // 2. Initialize SDL2, Windows DPI awareness, Window, Renderer & TTF Fonts
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }

        if (TTF_Init() == -1)
        {
            std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        m_window = SDL_CreateWindow(
            "MathStudio",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!m_window)
        {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        SDL_SetWindowBordered(m_window, SDL_TRUE);
        SDL_SetWindowResizable(m_window, SDL_TRUE);

#ifdef _WIN32
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        if (SDL_GetWindowWMInfo(m_window, &wmInfo))
        {
            HWND hwnd = wmInfo.info.win.window;
            BOOL useDarkMode = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
        }
#endif

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        if (!m_renderer)
        {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        std::vector<std::string> fontPaths;
        if (!m_config.fontPath.empty())
        {
            fontPaths.push_back(m_config.fontPath);
        }
        fontPaths.insert(fontPaths.end(), {"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                                           "/usr/share/fonts/TTF/DejaVuSans.ttf",
                                           "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                                           "/usr/share/fonts/TTF/LiberationSans-Regular.ttf",
                                           "/System/Library/Fonts/Arial.ttf",
                                           "C:\\Windows\\Fonts\\arial.ttf"});

        for (const auto &path : fontPaths)
        {
            if (path.empty())
                continue;
            m_font = TTF_OpenFont(path.c_str(), 14);
            if (m_font)
                break;
        }

        if (!m_font)
        {
            std::cerr << "Failed to load any font. Tried paths:" << std::endl;
            for (const auto &path : fontPaths)
            {
                std::cerr << "  " << path << std::endl;
            }
            std::cerr << "TTF Error: " << TTF_GetError() << std::endl;
            SDL_DestroyRenderer(m_renderer);
            SDL_DestroyWindow(m_window);
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        // 3. Initialize MathStudio UIManager & Register Dockable Panels
        ui::UIManager::instance().init(m_window, m_renderer);
        ui::UIManager::instance().registerPanel<ui::CanvasPanel>();
        ui::UIManager::instance().registerPanel<ui::ControlPanel>();
        ui::UIManager::instance().registerPanel<ui::VariableInspectorPanel>();
        ui::UIManager::instance().registerPanel<ui::PerformanceProfilerPanel>();
        ui::UIManager::instance().registerPanel<ui::DiagnosticsConsolePanel>();
        ui::UIManager::instance().registerPanel<ui::PlotPanel>();

        if (auto ctrl = ui::UIManager::instance().getPanel<ui::ControlPanel>())
        {
            if (!m_expressionLabels.empty())
            {
                ctrl->exprWidget.clear();
                for (const auto &lbl : m_expressionLabels)
                {
                    ctrl->exprWidget.addExpression(lbl);
                }
            }
        }

        // 4. Initial Viewport Auto-fit
        double minY = std::numeric_limits<double>::infinity();
        double maxY = -std::numeric_limits<double>::infinity();
        for (const auto &expr : m_expressions)
        {
            for (int i = 0; i <= 300; ++i)
            {
                double x = m_xmin + (m_xmax - m_xmin) * i / 300.0;
                double y = expr.eval(x);
                if (std::isfinite(y) && std::abs(y) <= Y_AUTO_CLAMP)
                {
                    minY = (std::min)(minY, y);
                    maxY = (std::max)(maxY, y);
                }
            }
        }
        if (std::isfinite(minY) && std::isfinite(maxY) && minY < maxY)
        {
            double pad = 0.1 * (maxY - minY);
            m_ymin = minY - pad;
            m_ymax = maxY + pad;
        }

        setScaleMode(m_config.scaleMode);
        setDiscontinuityThreshold(m_config.discThreshold);

        return true;
    }

    void Application::processEvents(SDL_Event &event)
    {
        if (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
                event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL)
            {
                return;
            }
        }

        if (event.type == SDL_QUIT)
        {
            m_running = false;
        }

        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                event.window.event == SDL_WINDOWEVENT_EXPOSED ||
                event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
                event.window.event == SDL_WINDOWEVENT_RESTORED)
            {
                m_needsRedraw = true;
            }
            else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                m_running = false;
            }
        }

        if (event.type == SDL_MOUSEWHEEL)
        {
            int zoomDir = (event.wheel.y > 0) ? -1 : 1;
            double zoomFactor = std::pow(m_config.zoomStep, zoomDir);

            double currentXRange = m_xmax - m_xmin;
            double currentYRange = m_ymax - m_ymin;

            double newXRange = currentXRange * zoomFactor;
            double newYRange = currentYRange * zoomFactor;

            if (newXRange >= m_config.zoomMin && newYRange >= m_config.zoomMin &&
                newXRange <= m_config.zoomMax && newYRange <= m_config.zoomMax)
            {
                double cx = (m_xmin + m_xmax) * 0.5;
                double cy = (m_ymin + m_ymax) * 0.5;

                m_xmin = cx - newXRange * 0.5;
                m_xmax = cx + newXRange * 0.5;
                m_ymin = cy - newYRange * 0.5;
                m_ymax = cy + newYRange * 0.5;

                m_rootsDirty = true;
                m_extremaDirty = true;
                m_needsRedraw = true;
            }
        }

        if (event.type == SDL_KEYDOWN)
        {
            double pan = 0.1 * (m_xmax - m_xmin);

            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                m_xmin -= pan;
                m_xmax -= pan;
                break;
            case SDLK_RIGHT:
                m_xmin += pan;
                m_xmax += pan;
                break;
            case SDLK_UP:
                m_ymin += pan;
                m_ymax += pan;
                break;
            case SDLK_ESCAPE:
                m_running = false;
                break;
            case SDLK_DOWN:
                m_ymin -= pan;
                m_ymax -= pan;
                break;
            case SDLK_t:
                m_showTangent = !m_showTangent;
                m_needsRedraw = true;
                break;
            case SDLK_TAB:
                if (!m_expressions.empty())
                {
                    m_activeExpr = (m_activeExpr + 1) % m_expressions.size();
                    m_needsRedraw = true;
                }
                break;
            case SDLK_g:
                m_showGrid = !m_showGrid;
                m_needsRedraw = true;
                break;
            case SDLK_r:
                m_showRoots = !m_showRoots;
                m_needsRedraw = true;
                break;
            case SDLK_e:
                m_showExtrema = !m_showExtrema;
                m_needsRedraw = true;
                break;
            case SDLK_F11:
            {
                Uint32 flags = SDL_GetWindowFlags(m_window);
                if (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP))
                {
                    SDL_SetWindowFullscreen(m_window, 0);
                    SDL_SetWindowBordered(m_window, SDL_TRUE);
                    SDL_SetWindowResizable(m_window, SDL_TRUE);
                    SDL_SetWindowSize(m_window, 1280, 720);
                    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                }
                else
                {
                    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
                m_needsRedraw = true;
                break;
            }
            }

            m_rootsDirty = true;
            m_needsRedraw = true;
            m_extremaDirty = true;
        }

        if (event.type == SDL_MOUSEMOTION)
        {
            m_mouseX = event.motion.x;
            m_mouseY = event.motion.y;
            m_needsRedraw = true;
        }
    }

    int Application::run()
    {
        SDL_Event event;
        core::FunctionRegistry &fReg = core::FunctionRegistry::instance();
        DiagnosticsReporter diag;
        core::EvaluationContext evalContext(m_config.cliVars, fReg, diag);

        while (m_running)
        {
            while (SDL_PollEvent(&event))
            {
                ui::UIManager::instance().processEvent(&event);
                processEvents(event);
            }

            // Begin ImGui Frame & Dockspace
            ui::UIManager::instance().beginFrame();
            ui::UIManager::instance().setupDockspace();

            // Clear frame buffer
            SDL_SetRenderDrawColor(m_renderer, 20, 22, 28, 255);
            SDL_RenderClear(m_renderer);

            // Render ImGui UI dockable panels & Scientific Plotter on top
            ui::UIManager::instance().render(evalContext);

            SDL_RenderPresent(m_renderer);
            m_needsRedraw = false;

            // Yield CPU slice without artificial 16ms sleep lag
            SDL_Delay(1);
        }

        return 0;
    }

    void Application::shutdown()
    {
        ui::UIManager::instance().shutdown();

        if (m_font)
        {
            TTF_CloseFont(m_font);
            m_font = nullptr;
        }
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        TTF_Quit();
        SDL_Quit();

        cli::CliRunner::printLeakReport(m_config);
    }

} // namespace mathstudio::app
