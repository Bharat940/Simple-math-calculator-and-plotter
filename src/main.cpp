#include "cli/CliParser.hpp"
#include "cli/CliRunner.hpp"
#include "app/Application.hpp"
#include <string>

// ============================================================================
// MATHSTUDIO MAIN ENTRY POINT (Clean modular Architecture)
// ============================================================================
int main(int argc, char **argv)
{
    mathstudio::cli::CliConfig config;
    std::string initialExprStr = "x^2";
    int exitCode = 0;

    // 1. Parse Command Line Arguments & Option Flags
    if (!mathstudio::cli::CliParser::parse(argc, argv, config, initialExprStr, exitCode))
    {
        return exitCode;
    }

    // 2. Dispatch One-Shot CLI Modes (-e, -s, -i, -d)
    if (config.mode != mathstudio::cli::CliMode::Gui)
    {
        return mathstudio::cli::CliRunner::execute(config);
    }

    // 3. Launch Interactive ImGui & ImPlot GUI Engine
    mathstudio::app::Application app(config, initialExprStr);
    if (!app.init())
    {
        return 1;
    }

    return app.run();
}