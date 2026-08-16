#pragma once

#include "cli/CliParser.hpp"

namespace mathstudio::cli
{

    class CliRunner
    {
    public:
        static int execute(CliConfig &cfg);
        static void printLeakReport(const CliConfig &cfg);
    };

} // namespace mathstudio::cli
