#include "cli/CliParser.hpp"
#include <iostream>
#include <cmath>
#include <cctype>

namespace mathstudio::cli
{

    int CliParser::cliError(const std::string &msg, int code)
    {
        std::cerr << "error: " << msg << std::endl;
        return code;
    }

    std::vector<std::string> CliParser::splitExpressions(const std::string &s)
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

    bool CliParser::validateExpression(const std::string &expr)
    {
        if (expr.length() > 1000)
        {
            return false;
        }

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

        int parenDepth = 0;
        for (char c : expr)
        {
            if (c == '(')
            {
                parenDepth++;
                if (parenDepth > 10)
                    return false;
            }
            else if (c == ')')
            {
                parenDepth--;
                if (parenDepth < 0)
                    return false;
            }
        }
        return parenDepth == 0;
    }

    void CliParser::printHelp()
    {
        std::cout << "MathStudio — Math Function Plotter & Solver\n"
                  << "============================================\n\n"
                  << "Usage:\n"
                  << "  mathstudio [options] \"expression\"      Plot one or more functions (comma-separated)\n"
                  << "  mathstudio -e \"expr\"                  Evaluate expression at x = 0\n"
                  << "  mathstudio -d \"expr\"                  Compute numerical derivative d/dx at x = 0\n"
                  << "  mathstudio -s \"expr\"                  Find roots (x where expr = 0)\n"
                  << "  mathstudio -i \"f(x)\" \"g(x)\"           Find intersections where f(x) = g(x)\n"
                  << "  mathstudio --help                     Show this help screen\n\n"
                  << "Options:\n"
                  << "  --var <name>=<value>    Set parameter variable (or -v name=value, e.g. --var a=5.0)\n"
                  << "  --range xmin xmax       Set range for solver/evaluator (default: -100 to 100)\n"
                  << "  --step value            Set iteration step size for solver (default: 0.1)\n"
                  << "  --precision value       Set target numeric precision (default: 1e-6)\n"
                  << "  --zoom-step value       Scroll zoom sensitivity (1.0 - 2.0, default: 1.1)\n"
                  << "  --zoom-min value        Minimum zoom-in range (default: 0.01)\n"
                  << "  --zoom-max value        Maximum zoom-out range (default: 500.0)\n"
                  << "  --scale <mode>          Grid scaling mode: auto | fixed | loose | dense\n"
                  << "  --font <path>           Path to a custom .ttf font file\n"
                  << "  --verbose               Show detailed convergence data for solver\n\n"
                  << "Variables:\n"
                  << "  x   standard independent variable (e.g. sin(x))\n"
                  << "  t   time variable for signals (e.g. sin(2*pi*t))\n"
                  << "  n   discrete index variable (e.g. sin(n*pi/4))\n"
                  << "  a..z custom parameter variables (e.g. --var a=5.0 --var b=2.0)\n\n"
                  << "Constants: pi, e, phi, tau (= 2*pi)\n\n"
                  << "Examples:\n"
                  << "  mathstudio \"sin(x), cos(x)\"\n"
                  << "  mathstudio -e \"a * cos(b * x)\" --var a=5.0 --var b=2.0\n"
                  << "  mathstudio -s \"a * x^2 - b\" --var a=1 --var b=9 --range -5 5\n"
                  << "  mathstudio \"a * sin(b * x)\" --var a=3.5 --var b=1.2\n"
                  << "  mathstudio --scale fixed \"x^2\"\n";
    }

    bool CliParser::parse(int argc, char **argv, CliConfig &cfg, std::string &exprStr, int &exitCode)
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--help")
            {
                printHelp();
                exitCode = 0;
                return false;
            }
            else if (arg == "--range")
            {
                if (i + 2 >= argc)
                {
                    exitCode = cliError("--range requires xmin xmax", 2);
                    return false;
                }
                try
                {
                    cfg.xmin = std::stod(argv[++i]);
                    cfg.xmax = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--range requires numeric values", 2);
                    return false;
                }
                if (cfg.xmin >= cfg.xmax)
                {
                    exitCode = cliError("invalid range: xmin >= xmax", 2);
                    return false;
                }
            }
            else if (arg == "--step")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--step requires value", 2);
                    return false;
                }
                try
                {
                    cfg.step = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--step requires a numeric value", 2);
                    return false;
                }
                if (cfg.step <= 0)
                {
                    exitCode = cliError("step must be > 0", 2);
                    return false;
                }
            }
            else if (arg == "--precision")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--precision requires value", 2);
                    return false;
                }
                try
                {
                    cfg.precision = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--precision requires a numeric value", 2);
                    return false;
                }
                if (cfg.precision <= 0)
                {
                    exitCode = cliError("precision must be > 0", 2);
                    return false;
                }
            }
            else if (arg == "--zoom-step")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--zoom-step requires a value", 2);
                    return false;
                }
                try
                {
                    cfg.zoomStep = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--zoom-step requires a numeric value", 2);
                    return false;
                }
                if (cfg.zoomStep <= 1.0 || cfg.zoomStep > 2.0)
                {
                    exitCode = cliError("--zoom-step must be in range (1.0, 2.0]", 2);
                    return false;
                }
            }
            else if (arg == "--zoom-min")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--zoom-min requires a value", 2);
                    return false;
                }
                try
                {
                    cfg.zoomMin = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--zoom-min requires a numeric value", 2);
                    return false;
                }
                if (cfg.zoomMin <= 0)
                {
                    exitCode = cliError("--zoom-min must be > 0", 2);
                    return false;
                }
            }
            else if (arg == "--zoom-max")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--zoom-max requires a value", 2);
                    return false;
                }
                try
                {
                    cfg.zoomMax = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--zoom-max requires a numeric value", 2);
                    return false;
                }
                if (cfg.zoomMax <= cfg.zoomMin)
                {
                    exitCode = cliError("--zoom-max must be > zoom-min", 2);
                    return false;
                }
            }
            else if (arg == "--verbose")
            {
                cfg.verbose = true;
            }
            else if (arg == "--disc-threshold")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--disc-threshold requires a value", 2);
                    return false;
                }
                try
                {
                    cfg.discThreshold = std::stod(argv[++i]);
                }
                catch (const std::exception &)
                {
                    exitCode = cliError("--disc-threshold requires a numeric value", 2);
                    return false;
                }
                if (cfg.discThreshold <= 0)
                {
                    exitCode = cliError("--disc-threshold must be > 0", 2);
                    return false;
                }
            }
            else if (arg == "--scale")
            {
                if (i + 1 >= argc)
                {
                    exitCode = cliError("--scale requires auto|fixed|loose|dense", 2);
                    return false;
                }

                std::string mode = argv[++i];
                if (mode == "auto")
                    cfg.scaleMode = ScaleMode::Auto;
                else if (mode == "fixed")
                    cfg.scaleMode = ScaleMode::Fixed;
                else if (mode == "loose")
                    cfg.scaleMode = ScaleMode::Loose;
                else if (mode == "dense")
                    cfg.scaleMode = ScaleMode::Dense;
                else
                {
                    exitCode = cliError("invalid scale mode", 2);
                    return false;
                }
            }
            else if ((arg == "--var" || arg == "-v") && i + 1 < argc)
            {
                std::string pair = argv[++i];
                size_t eqPos = pair.find('=');
                if (eqPos != std::string::npos)
                {
                    std::string varName = pair.substr(0, eqPos);
                    try
                    {
                        double varVal = std::stod(pair.substr(eqPos + 1));
                        cfg.cliVars.set(varName, MathValue::real(varVal));
                    }
                    catch (...)
                    {
                        exitCode = cliError("invalid numeric value for --var " + pair, 2);
                        return false;
                    }
                }
                else
                {
                    exitCode = cliError("--var requires format name=value (e.g. --var a=5.0)", 2);
                    return false;
                }
            }
            else if (arg == "--check-leaks" || arg == "--debug-leaks")
            {
                cfg.checkLeaks = true;
            }
            else if (arg == "-e" && i + 1 < argc)
            {
                cfg.mode = CliMode::Eval;
                cfg.evalExpr = argv[++i];
            }
            else if (arg == "-d" && i + 1 < argc)
            {
                cfg.mode = CliMode::Derivative;
                cfg.derivExpr = argv[++i];
            }
            else if (arg == "-s" && i + 1 < argc)
            {
                cfg.mode = CliMode::Solve;
                cfg.solveExpr = argv[++i];
            }
            else if (arg == "-i" && i + 2 < argc)
            {
                cfg.mode = CliMode::Intersect;
                cfg.intersectF = argv[++i];
                cfg.intersectG = argv[++i];
            }
            else if (arg == "-p" && i + 1 < argc)
            {
                exprStr = argv[++i];
            }
            else if (arg[0] != '-')
            {
                exprStr = arg;
            }
            else
            {
                exitCode = cliError("unknown option: " + arg + "\nRun with --help to view valid options.", 2);
                return false;
            }
        }

        return true;
    }

} // namespace mathstudio::cli
