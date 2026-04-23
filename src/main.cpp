#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include "math/numerical.h"
#include "math/solver.h"
#include "renderer.h"
#include "math/expression.h"

// Hard zoom limits (defaults)
constexpr double DEFAULT_MIN_VIEW_RANGE = 0.01;
constexpr double DEFAULT_MAX_VIEW_RANGE = 500.0;
constexpr double Y_AUTO_CLAMP = 10.0;

static bool validateExpression(const std::string &expr) {
    // Length limit
    if (expr.length() > 1000) {
        return false;
    }
    
    // Character whitelist
    const std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.+-*/^(),x ";
    for (char c : expr) {
        if (allowedChars.find(c) == std::string::npos) {
            return false;
        }
    }
    
    // Check for balanced parentheses
    int parenCount = 0;
    for (char c : expr) {
        if (c == '(') parenCount++;
        if (c == ')') parenCount--;
        if (parenCount < 0) return false;
    }
    if (parenCount != 0) return false;
    
    return true;
}

static int cliError(const std::string &msg, int code)
{
    std::cerr << "error: " << msg << std::endl;
    return code;
}

static std::vector<std::string> splitExpressions(const std::string &s)
{
    std::vector<std::string> out;
    std::string current;
    int depth = 0;

    for (char c : s)
    {
        if (c == '(')
        {
            depth++;
            current.push_back(c);
        }
        else if (c == ')')
        {
            depth--;
            current.push_back(c);
        }
        else if (c == ',' && depth == 0)
        {
            if (!current.empty())
            {
                out.push_back(current);
                current.clear();
            }
        }
        else if (!std::isspace(static_cast<unsigned char>(c)))
        {
            current.push_back(c);
        }
    }

    if (!current.empty())
    {
        out.push_back(current);
    }

    return out;
}

static bool validateExpression(const std::string &expr)
{
    // Length limit
    if (expr.length() > 1000)
    {
        return false;
    }

    // Character whitelist
    static const std::string allowedChars = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "+-*/^(),.x ";

    for (char c : expr)
    {
        if (allowedChars.find(c) == std::string::npos)
        {
            return false;
        }
    }

    // Basic structure checks
    int parenDepth = 0;
    bool hasX = false;

    for (size_t i = 0; i < expr.length(); ++i)
    {
        char c = expr[i];
        if (c == '(')
        {
            parenDepth++;
            if (parenDepth > 10) return false; // Max nesting
        }
        else if (c == ')')
        {
            parenDepth--;
            if (parenDepth < 0) return false;
        }
        else if (c == 'x')
        {
            hasX = true;
        }
    }

    if (parenDepth != 0) return false;

    return true;
}

struct CliConfig
{
    double xmin = -100.0;
    double xmax = 100.0;
    double step = 0.1;
    double precision = 1e-6;
    bool verbose = false;

    double zoomStep = 1.1;
    double zoomMin = DEFAULT_MIN_VIEW_RANGE;
    double zoomMax = DEFAULT_MAX_VIEW_RANGE;

    ScaleMode scaleMode = ScaleMode::Auto;
    std::string fontPath;
};

void printHelp()
{
    std::cout << "Usage:\n"
                 "  plotter \"expr\"              Plot expression in GUI\n"
                 "  plotter -e \"expr\"           Evaluate expression at x = 0\n"
                 "  plotter -s \"expr\"           Solve expr = 0\n"
                 "  plotter -i \"f(x)\" \"g(x)\"  Solve f(x) = g(x)\n"
                 "  plotter -p \"expr\"           Explicit GUI plot\n"
                 "  plotter --help                Show this help\n\n"
                 "Options:\n"
                 "  --range xmin xmax       Set solving range (CLI only)\n"
                 "  --step value            Set solver step size\n"
                 "  --precision value       Set numeric precision\n"
                 "  --zoom-step value       Zoom sensitivity (default 1.1, range: 1.0-2.0)\n"
                 "  --zoom-min value        Minimum view range (default 0.01)\n"
                 "  --zoom-max value        Maximum view range (default 500)\n"
                 "  --verbose               Show detailed output\n"
                 "  --scale mode            Grid/label scale: auto|fixed|loose|dense\n"
                 "  --font path             Font file path for GUI text\n";
}

int main(int argc, char **argv)
{
    std::string exprStr = "x^2";
    CliConfig cfg;

    // CLI parsing
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help")
        {
            printHelp();
            return 0;
        }
        else if (arg == "--range")
        {
            if (i + 2 >= argc)
            {
                return cliError("--range requires xmin xmax", 2);
            }
            cfg.xmin = std::stod(argv[++i]);
            cfg.xmax = std::stod(argv[++i]);
            if (cfg.xmin >= cfg.xmax)
            {
                return cliError("invalid range: xmin >= xmax", 2);
            }
        }
        else if (arg == "--step")
        {
            if (i + 1 >= argc)
            {
                return cliError("--step requires value", 2);
            }
            cfg.step = std::stod(argv[++i]);
            if (cfg.step <= 0)
            {
                return cliError("step must be > 0", 2);
            }
        }
        else if (arg == "--precision")
        {
            if (i + 1 >= argc)
            {
                return cliError("--precision requires value", 2);
            }
            cfg.precision = std::stod(argv[++i]);
            if (cfg.precision <= 0)
            {
                return cliError("precision must be > 0", 2);
            }
        }
        else if (arg == "--zoom-step")
        {
            if (i + 1 >= argc)
            {
                return cliError("--zoom-step requires a value", 2);
            }
            cfg.zoomStep = std::stod(argv[++i]);
            if (cfg.zoomStep <= 1.0 || cfg.zoomStep > 2.0)
            {
                return cliError("--zoom-step must be in range (1.0, 2.0]", 2);
            }
        }
        else if (arg == "--zoom-min")
        {
            if (i + 1 >= argc)
            {
                return cliError("--zoom-min requires a value", 2);
            }
            cfg.zoomMin = std::stod(argv[++i]);
            if (cfg.zoomMin <= 0)
            {
                return cliError("--zoom-min must be > 0", 2);
            }
        }
        else if (arg == "--zoom-max")
        {
            if (i + 1 >= argc)
            {
                return cliError("--zoom-max requires a value", 2);
            }
            cfg.zoomMax = std::stod(argv[++i]);
            if (cfg.zoomMax <= cfg.zoomMin)
            {
                return cliError("--zoom-max must be > zoom-min", 2);
            }
        }
        else if (arg == "--verbose")
        {
            cfg.verbose = true;
        }
        else if (arg == "--scale")
        {
            if (i + 1 >= argc)
            {
                return cliError("--scale requires auto|fixed|loose|dense", 2);
            }

            std::string mode = argv[++i];

            if (mode == "auto")
            {
                cfg.scaleMode = ScaleMode::Auto;
            }
            else if (mode == "fixed")
            {
                cfg.scaleMode = ScaleMode::Fixed;
            }
            else if (mode == "loose")
            {
                cfg.scaleMode = ScaleMode::Loose;
            }
            else if (mode == "dense")
            {
                cfg.scaleMode = ScaleMode::Dense;
            }
            else
            {
                return cliError("invalid scale mode", 2);
            }
        }
        else if (arg == "--font")
        {
            if (i + 1 >= argc)
            {
                return cliError("--font requires a path", 2);
            }
            cfg.fontPath = argv[++i];
        }
        else if (arg == "-e" && i + 1 < argc)
        {
            try
            {
                Expression e(argv[++i]);
                auto r = e.evalSafe(0.0);
                if (!r.ok)
                {
                    return cliError(r.error, 3);
                }
                std::cout << std::setprecision(
                                 static_cast<int>(-std::log10(cfg.precision)))
                          << r.value << std::endl;
                return 0;
            }
            catch (const std::exception &ex)
            {
                return cliError(ex.what(), 2);
            }
        }
        else if (arg == "-s" && i + 1 < argc)
        {
            try
            {
                Expression e(argv[++i]);
                if (cfg.verbose)
                {
                    auto results = findRootsDetailed(
                        e, cfg.xmin, cfg.xmax, cfg.step, cfg.precision);
                    if (results.empty())
                    {
                        return cliError("no real roots found", 4);
                    }
                    for (const auto &r : results)
                    {
                        std::cout << "x = " << r.x << "\n";
                        std::cout << "  converged: " << (r.converged ? "yes" : "no") << "\n";
                        std::cout << "  iterations: " << r.iterations << "\n";
                        std::cout << "  residual: " << r.residual << "\n\n";
                    }
                }
                else
                {
                    auto roots = findRoots(
                        e, cfg.xmin, cfg.xmax, cfg.step, cfg.precision);
                    if (roots.empty())
                    {
                        return cliError("no real roots found", 4);
                    }
                    for (double r : roots)
                    {
                        std::cout << r << " ";
                    }
                    std::cout << std::endl;
                }
                return 0;
            }
            catch (const std::exception &ex)
            {
                return cliError(ex.what(), 2);
            }
        }
        else if (arg == "-i" && i + 2 < argc)
        {
            try
            {
                Expression f(argv[++i]);
                Expression g(argv[++i]);
                auto intersections = findIntersections(f, g, cfg.xmin, cfg.xmax, cfg.step, cfg.precision);
                if (intersections.empty())
                {
                    return cliError("no intersections found", 4);
                }
                for (double x : intersections)
                {
                    std::cout << x << " ";
                }
                std::cout << std::endl;
                return 0;
            }
            catch (const std::exception &ex)
            {
                return cliError(ex.what(), 2);
            }
        }
        else if (arg == "-p" && i + 1 < argc)
        {
            exprStr = argv[++i];
        }
        else if (arg[0] != '-')
        {
            exprStr = arg;
        }
    }

    // Parse expressions for GUI
    std::vector<Expression> expressions;
    std::vector<std::string> expressionLabels;

    try
    {
        auto parts = splitExpressions(exprStr);
        for (const auto &p : parts)
        {
            expressions.emplace_back(p);
            expressionLabels.push_back(p);
        }
    }
    catch (const std::exception &ex)
    {
        return cliError(ex.what(), 2);
    }

    // SDL setup
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Function Plotter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Font loading with fallbacks
    std::vector<std::string> fontPaths;
    if (!cfg.fontPath.empty()) {
        fontPaths.push_back(cfg.fontPath);
    }
    fontPaths.insert(fontPaths.end(), {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Arial.ttf",  // macOS
        "C:\\Windows\\Fonts\\arial.ttf"     // Windows
    });

    TTF_Font *font = nullptr;
    for (const auto &path : fontPaths) {
        font = TTF_OpenFont(path.c_str(), 12);
        if (font) {
            break;
        }
    }

    if (!font) {
        std::cerr << "Failed to load any font. Tried paths:" << std::endl;
        for (const auto &path : fontPaths) {
            std::cerr << "  " << path << std::endl;
        }
        std::cerr << "TTF Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Viewport initialization
    double xmin = -10.0, xmax = 10.0;
    double ymin = -10.0, ymax = 10.0;

    std::vector<std::vector<double>> roots(expressions.size());
    std::vector<std::vector<double>> extrema(expressions.size());

    // Auto-fit Y range
    double minY = std::numeric_limits<double>::infinity();
    double maxY = -std::numeric_limits<double>::infinity();

    for (const auto &expr : expressions)
    {
        for (int i = 0; i <= 300; ++i)
        {
            double x = xmin + (xmax - xmin) * i / 300.0;
            double y = expr.eval(x);
            if (std::isfinite(y) && std::abs(y) <= Y_AUTO_CLAMP)
            {
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
    }

    if (std::isfinite(minY) && std::isfinite(maxY) && minY < maxY)
    {
        double pad = 0.1 * (maxY - minY);
        ymin = minY - pad;
        ymax = maxY + pad;
    }

    bool extremaDirty = true;
    bool showTangent = false;
    bool showGrid = true;
    bool showExtrema = false;
    bool showRoots = false;
    size_t activeExpr = 0;

    // UI state
    bool running = true;
    bool needsRedraw = true;
    bool rootsDirty = false;

    int mouseX = 0, mouseY = 0;

    std::string lastCoordText;
    SDL_Texture *coordTexture = nullptr;
    SDL_Rect coordRect{};

    SDL_Event event;

    setScaleMode(cfg.scaleMode);

    // Main loop
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            if (event.type == SDL_MOUSEWHEEL)
            {
                int zoomDir = (event.wheel.y > 0) ? -1 : 1;
                double zoomFactor = std::pow(cfg.zoomStep, zoomDir);

                double currentXRange = xmax - xmin;
                double currentYRange = ymax - ymin;

                double newXRange = currentXRange * zoomFactor;
                double newYRange = currentYRange * zoomFactor;

                // HARD STOP: zoom-in
                if (newXRange < cfg.zoomMin || newYRange < cfg.zoomMin)
                {
                    continue;
                }

                // HARD STOP: zoom-out
                if (newXRange > cfg.zoomMax || newYRange > cfg.zoomMax)
                {
                    continue;
                }

                double cx = (xmin + xmax) * 0.5;
                double cy = (ymin + ymax) * 0.5;

                xmin = cx - newXRange * 0.5;
                xmax = cx + newXRange * 0.5;
                ymin = cy - newYRange * 0.5;
                ymax = cy + newYRange * 0.5;

                rootsDirty = true;
                extremaDirty = true;
                needsRedraw = true;
            }

            if (event.type == SDL_KEYDOWN)
            {
                double pan = 0.1 * (xmax - xmin);

                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    xmin -= pan;
                    xmax -= pan;
                    break;
                case SDLK_RIGHT:
                    xmin += pan;
                    xmax += pan;
                    break;
                case SDLK_UP:
                    ymin += pan;
                    ymax += pan;
                    break;
                case SDLK_DOWN:
                    ymin -= pan;
                    ymax -= pan;
                    break;
                case SDLK_t:
                    showTangent = !showTangent;
                    needsRedraw = true;
                    break;
                case SDLK_TAB:
                    if (!expressions.empty())
                    {
                        activeExpr = (activeExpr + 1) % expressions.size();
                        needsRedraw = true;
                    }
                    break;
                case SDLK_g:
                    showGrid = !showGrid;
                    needsRedraw = true;
                    break;
                case SDLK_r:
                    showRoots = !showRoots;
                    needsRedraw = true;
                    break;
                case SDLK_e:
                    showExtrema = !showExtrema;
                    needsRedraw = true;
                    break;
                }

                rootsDirty = true;
                needsRedraw = true;
                extremaDirty = true;
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                needsRedraw = true;
            }
        }

        if (needsRedraw)
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);

            if (rootsDirty)
            {
                for (size_t i = 0; i < expressions.size(); ++i)
                {
                    roots[i] = findRoots(expressions[i], xmin, xmax);
                }
                rootsDirty = false;
            }

            if (extremaDirty)
            {
                for (size_t i = 0; i < expressions.size(); ++i)
                {
                    extrema[i] = findExtrema(expressions[i], xmin, xmax);
                }
                extremaDirty = false;
            }

            double mouseMathX = screenToMathX(mouseX, width, xmin, xmax);
            double mouseMathY = screenToMathY(mouseY, height, ymin, ymax);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            if (showGrid)
            {
                drawGrid(renderer, width, height, xmin, xmax, ymin, ymax);
            }
            drawAxes(renderer, width, height, xmin, xmax, ymin, ymax);
            drawAxisLabels(renderer, font, width, height, xmin, xmax, ymin, ymax);
            drawLegend(renderer, font, expressionLabels, activeExpr);

            for (size_t i = 0; i < expressions.size(); ++i)
            {
                drawFunction(renderer, expressions[i], i, i == activeExpr, width, height, xmin, xmax, ymin, ymax);
            }

            if (showTangent && !expressions.empty())
            {
                const Expression &e = expressions[activeExpr];
                Line tangent = tangentAt(e, mouseMathX);
                SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
                drawLine(renderer, tangent, width, height, xmin, xmax, ymin, ymax);
            }

            if (!expressions.empty())
            {
                if (showRoots)
                {
                    drawRoots(renderer, roots[activeExpr], width, height, xmin, xmax, ymin, ymax);
                }
                if (showExtrema)
                {
                    drawExtrema(renderer, expressions[activeExpr], extrema[activeExpr], width, height, xmin, xmax, ymin, ymax);
                }
            }

            // Mouse coordinate overlay
            std::string coordText =
                "x=" + std::to_string(mouseMathX).substr(0, 6) +
                ", y=" + std::to_string(mouseMathY).substr(0, 6);

            if (coordText != lastCoordText)
            {
                if (coordTexture)
                {
                    SDL_DestroyTexture(coordTexture);
                }

                SDL_Color color = {255, 255, 0, 255};
                SDL_Surface *surface = TTF_RenderText_Solid(font, coordText.c_str(), color);
                coordTexture = SDL_CreateTextureFromSurface(renderer, surface);
                coordRect = {mouseX + 10, mouseY + 10, surface->w, surface->h};
                SDL_FreeSurface(surface);
                lastCoordText = coordText;
            }

            if (coordTexture)
            {
                SDL_RenderCopy(renderer, coordTexture, nullptr, &coordRect);
            }

            SDL_RenderPresent(renderer);
            needsRedraw = false;
        }

        SDL_Delay(16);
    }

    // Cleanup
    if (coordTexture)
        SDL_DestroyTexture(coordTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
