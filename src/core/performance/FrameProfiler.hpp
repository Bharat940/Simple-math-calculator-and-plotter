#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "core/performance/PerformanceClock.hpp"

namespace mathstudio::core::performance
{
    enum class ProfileZone : uint8_t
    {
        MathEvaluation,
        PlotSampling,
        DerivativeEval,
        RootExtremaScan,
        ImGuiLayout,
        ImPlotRender,
        SDLRenderPresent,
        Count
    };

    struct ZoneStats
    {
        const char* name = nullptr;
        double currentAccumMs = 0.0;
        double lastFrameMs = 0.0;
        double avgMs = 0.0;
        double minMs = 1e9;
        double maxMs = 0.0;
        uint64_t callCount = 0;
    };

    struct FrameTelemetry
    {
        double frameTimeMs = 0.0;
        double fps = 0.0;
        double p50Ms = 0.0;
        double p95Ms = 0.0;
        double p99Ms = 0.0;
        double minMs = 0.0;
        double maxMs = 0.0;

        // Observable Verification Counters
        uint32_t astEvaluations = 0;
        uint32_t plotPointsSampled = 0;
        uint32_t cacheHits = 0;
        uint32_t cacheMisses = 0;
        uint32_t heapAllocations = 0;
    };

    class FrameProfiler
    {
    public:
        static FrameProfiler& instance();

        void beginFrame();
        void beginZone(ProfileZone zone);
        void endZone(ProfileZone zone);
        void endFrame();

        // Observable Verification Counters
        void recordAstEvaluations(uint32_t count) { m_currentFrame.astEvaluations += count; }
        void recordPlotPoints(uint32_t count) { m_currentFrame.plotPointsSampled += count; }
        void recordCacheHit() { m_currentFrame.cacheHits++; }
        void recordCacheMiss() { m_currentFrame.cacheMisses++; }
        void recordAllocation() { m_currentFrame.heapAllocations++; }

        const FrameTelemetry& getTelemetry() const { return m_activeTelemetry; }
        const std::array<ZoneStats, static_cast<size_t>(ProfileZone::Count)>& getZones() const { return m_zones; }
        const std::array<float, 300>& getFrameHistory() const { return m_frameHistory; }
        size_t getRingIndex() const { return m_ringIndex; }
        size_t getTotalFrames() const { return m_totalFrames; }

        void reset();

    private:
        FrameProfiler();

        static constexpr size_t kHistorySize = 300;
        static constexpr size_t kZoneCount = static_cast<size_t>(ProfileZone::Count);

        std::array<ZoneStats, kZoneCount> m_zones{};
        std::array<uint64_t, kZoneCount> m_zoneStartTicks{};
        std::array<float, kHistorySize> m_frameHistory{};
        
        size_t m_ringIndex = 0;
        size_t m_totalFrames = 0;
        uint64_t m_frameStartTicks = 0;

        FrameTelemetry m_currentFrame{};
        FrameTelemetry m_activeTelemetry{};

        void computePercentiles();
    };

    class ScopedZone
    {
    public:
        explicit ScopedZone(ProfileZone zone) : m_zone(zone) {
            FrameProfiler::instance().beginZone(m_zone);
        }
        ~ScopedZone() {
            FrameProfiler::instance().endZone(m_zone);
        }
    private:
        ProfileZone m_zone;
    };
}

#if defined(MATHSTUDIO_ENABLE_PROFILING) && (MATHSTUDIO_ENABLE_PROFILING == 0)
#define PROFILE_ZONE(zone) ((void)0)
#else
#define PROFILE_ZONE(zone) mathstudio::core::performance::ScopedZone _zone_timer_##__LINE__(zone)
#endif
