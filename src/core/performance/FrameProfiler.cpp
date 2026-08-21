#include "core/performance/FrameProfiler.hpp"
#include <algorithm>
#include <cstring>

namespace mathstudio::core::performance
{
    FrameProfiler &FrameProfiler::instance()
    {
        static FrameProfiler s_instance;
        return s_instance;
    }

    FrameProfiler::FrameProfiler()
    {
        reset();
    }

    void FrameProfiler::reset()
    {
        m_zones[static_cast<size_t>(ProfileZone::MathEvaluation)].name = "Math Evaluation";
        m_zones[static_cast<size_t>(ProfileZone::PlotSampling)].name = "Plot Sampling";
        m_zones[static_cast<size_t>(ProfileZone::DerivativeEval)].name = "Derivative Eval";
        m_zones[static_cast<size_t>(ProfileZone::RootExtremaScan)].name = "Root / Extrema Scan";
        m_zones[static_cast<size_t>(ProfileZone::ImGuiLayout)].name = "ImGui Layout";
        m_zones[static_cast<size_t>(ProfileZone::ImPlotRender)].name = "ImPlot Render";
        m_zones[static_cast<size_t>(ProfileZone::SDLRenderPresent)].name = "SDL Render Present";

        for (auto &z : m_zones)
        {
            z.currentAccumMs = 0.0;
            z.lastFrameMs = 0.0;
            z.avgMs = 0.0;
            z.minMs = 1e9;
            z.maxMs = 0.0;
            z.callCount = 0;
        }

        m_zoneStartTicks.fill(0);
        m_frameHistory.fill(0.0f);
        m_ringIndex = 0;
        m_totalFrames = 0;
        m_currentFrame = FrameTelemetry{};
        m_activeTelemetry = FrameTelemetry{};
    }

    void FrameProfiler::beginFrame()
    {
        m_frameStartTicks = PerformanceClock::now();

        for (auto &z : m_zones)
        {
            z.currentAccumMs = 0.0;
        }

        m_currentFrame.astEvaluations = 0;
        m_currentFrame.plotPointsSampled = 0;
        m_currentFrame.cacheHits = 0;
        m_currentFrame.cacheMisses = 0;
        m_currentFrame.heapAllocations = 0;
    }

    void FrameProfiler::beginZone(ProfileZone zone)
    {
        size_t idx = static_cast<size_t>(zone);
        if (idx < kZoneCount)
        {
            m_zoneStartTicks[idx] = PerformanceClock::now();
        }
    }

    void FrameProfiler::endZone(ProfileZone zone)
    {
        size_t idx = static_cast<size_t>(zone);
        if (idx < kZoneCount && m_zoneStartTicks[idx] != 0)
        {
            uint64_t endTicks = PerformanceClock::now();
            double ms = PerformanceClock::elapsedMs(m_zoneStartTicks[idx], endTicks);
            m_zones[idx].currentAccumMs += ms;
            m_zones[idx].callCount++;

            // Exponential moving average / Min / Max
            if (m_zones[idx].avgMs == 0.0)
                m_zones[idx].avgMs = ms;
            else
                m_zones[idx].avgMs = m_zones[idx].avgMs * 0.95 + ms * 0.05;

            if (ms < m_zones[idx].minMs)
                m_zones[idx].minMs = ms;
            if (ms > m_zones[idx].maxMs)
                m_zones[idx].maxMs = ms;

            m_zoneStartTicks[idx] = 0;
        }
    }

    void FrameProfiler::endFrame()
    {
        uint64_t endTicks = PerformanceClock::now();
        double frameMs = PerformanceClock::elapsedMs(m_frameStartTicks, endTicks);

        // Snapshot accumulated zone timings for UI display
        for (auto &z : m_zones)
        {
            z.lastFrameMs = z.currentAccumMs;
        }

        m_frameHistory[m_ringIndex] = static_cast<float>(frameMs);
        m_ringIndex = (m_ringIndex + 1) % kHistorySize;
        m_totalFrames++;

        m_currentFrame.frameTimeMs = frameMs;
        m_currentFrame.fps = frameMs > 0.001 ? (1000.0 / frameMs) : 0.0;

        // Copy counters to active telemetry
        m_activeTelemetry.frameTimeMs = m_currentFrame.frameTimeMs;
        m_activeTelemetry.fps = m_currentFrame.fps;
        m_activeTelemetry.astEvaluations = m_currentFrame.astEvaluations;
        m_activeTelemetry.plotPointsSampled = m_currentFrame.plotPointsSampled;
        m_activeTelemetry.cacheHits = m_currentFrame.cacheHits;
        m_activeTelemetry.cacheMisses = m_currentFrame.cacheMisses;
        m_activeTelemetry.heapAllocations = m_currentFrame.heapAllocations;

        // Compute percentiles periodically every 60 frames
        if (m_totalFrames % 60 == 0 || m_totalFrames < 60)
        {
            computePercentiles();
        }
    }

    void FrameProfiler::computePercentiles()
    {
        size_t count = (std::min)(m_totalFrames, kHistorySize);
        if (count == 0)
            return;

        std::array<float, kHistorySize> sorted = m_frameHistory;
        std::sort(sorted.begin(), sorted.begin() + count);

        m_activeTelemetry.minMs = static_cast<double>(sorted[0]);
        m_activeTelemetry.maxMs = static_cast<double>(sorted[count - 1]);
        m_activeTelemetry.p50Ms = static_cast<double>(sorted[static_cast<size_t>(count * 0.50)]);
        m_activeTelemetry.p95Ms = static_cast<double>(sorted[static_cast<size_t>(count * 0.95)]);
        m_activeTelemetry.p99Ms = static_cast<double>(sorted[static_cast<size_t>(count * 0.99)]);
    }
}
