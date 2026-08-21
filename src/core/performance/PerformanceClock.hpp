#pragma once
#include <cstdint>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

namespace mathstudio::core::performance
{
    class PerformanceClock
    {
    public:
        static inline uint64_t now() noexcept
        {
#if defined(_WIN32)
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            return static_cast<uint64_t>(counter.QuadPart);
#elif defined(__APPLE__)
            return mach_absolute_time();
#else
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL + static_cast<uint64_t>(ts.tv_nsec);
#endif
        }

        static inline double elapsedMs(uint64_t startTicks, uint64_t endTicks) noexcept
        {
#if defined(_WIN32)
            static const double invFrequency = []()
            {
                LARGE_INTEGER freq;
                QueryPerformanceFrequency(&freq);
                return 1000.0 / static_cast<double>(freq.QuadPart);
            }();
            return static_cast<double>(endTicks - startTicks) * invFrequency;
#elif defined(__APPLE__)
            static const double timebaseFactor = []()
            {
                mach_timebase_info_data_t tb;
                mach_timebase_info(&tb);
                return (static_cast<double>(tb.numer) / static_cast<double>(tb.denom)) * 1e-6;
            }();
            return static_cast<double>(endTicks - startTicks) * timebaseFactor;
#else
            return static_cast<double>(endTicks - startTicks) * 1e-6;
#endif
        }
    };
}
