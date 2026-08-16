#pragma once

#include "core/VariableStore.hpp"
#include "renderer.h"
#include <string>
#include <vector>

namespace mathstudio::cli
{

    enum class CliMode
    {
        Gui,
        Eval,
        Solve,
        Intersect,
        Derivative
    };

    struct CliConfig
    {
        double xmin = -100.0;
        double xmax = 100.0;
        double step = 0.1;
        double precision = 1e-6;
        bool verbose = false;

        double zoomStep = 1.1;
        double zoomMin = 0.01;
        double zoomMax = 500.0;

        ScaleMode scaleMode = ScaleMode::Auto;
        double discThreshold = 10000.0;
        std::string fontPath;

        CliMode mode = CliMode::Gui;
        std::string evalExpr;
        std::string solveExpr;
        std::string intersectF;
        std::string intersectG;
        std::string derivExpr;

        VariableStore cliVars;
        bool checkLeaks = false;
    };

    class CliParser
    {
    public:
        static bool parse(int argc, char **argv, CliConfig &cfg, std::string &exprStr, int &exitCode);
        static void printHelp();
        static int cliError(const std::string &msg, int code);
        static std::vector<std::string> splitExpressions(const std::string &s);
        static bool validateExpression(const std::string &expr);
    };

} // namespace mathstudio::cli
