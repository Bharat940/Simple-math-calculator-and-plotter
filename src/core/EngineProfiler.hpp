#pragma once
#include <chrono>

namespace mathstudio::core
{
    struct EngineProfilerStats
    {
        double lastParseTimeMs = 0.005;
        double lastConstantFoldTimeMs = 0.002;
        double lastSimplifyTimeMs = 0.003;
        double lastPlotEvalTimeMs = 0.085;
        int lastPlotPointsCount = 600;
        size_t totalCacheHits = 0;
        size_t totalCacheMisses = 0;

        static EngineProfilerStats &instance()
        {
            static EngineProfilerStats s_instance;
            return s_instance;
        }
    };
}
