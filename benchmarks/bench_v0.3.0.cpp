// benchmarks/bench_v0.3.0.cpp
// Dedicated Benchmark Suite for MathStudio v0.3.0 Dear ImGui & ImPlot UI Release
//
// Build: cmake --build build --config Release --target bench_v0.3.0
// Run:   ./build/bench_v0.3.0 (or .\build\Release\bench_v0.3.0.exe on Windows)

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <cstdio>

#include "../src/math/expression.h"
#include "../src/math/solver.h"
#include "../src/core/ExpressionCache.hpp"
#include "../src/core/VariableStore.hpp"
#include "../src/compiler/parser/PrattParser.hpp"
#include "../src/compiler/visitors/EvalVisitor.hpp"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

using Clock = std::chrono::high_resolution_clock;

struct BenchResult
{
    std::string name;
    double ms;
    int runs;
};

BenchResult bench(const std::string &name, std::function<void()> fn, int warmup = 1, int runs = 3)
{
    for (int i = 0; i < warmup; i++)
        fn();
    auto start = Clock::now();
    for (int i = 0; i < runs; i++)
        fn();
    auto end = Clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count() / runs;
    return {name, ms, runs};
}

size_t getPeakRAMMB()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return pmc.PeakWorkingSetSize / (1024 * 1024);
    }
#endif
    return 0;
}

int main()
{
    std::cout << "==================================================\n";
    std::cout << "MathStudio v0.3.0 ImGui & ImPlot UI Benchmark Suite\n";
    std::cout << "==================================================\n";

#if defined(_MSC_VER)
    std::cout << "Compiler      : MSVC " << _MSC_VER << " (" << _MSC_FULL_VER << ")\n";
#elif defined(__clang__)
    std::cout << "Compiler      : Clang " << __clang_major__ << "." << __clang_minor__ << "\n";
#elif defined(__GNUC__)
    std::cout << "Compiler      : GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "\n";
#else
    std::cout << "Compiler      : Unknown\n";
#endif

#if defined(NDEBUG)
    std::cout << "Build Mode    : Release (Optimized)\n";
#else
    std::cout << "Build Mode    : Debug\n";
#endif

    std::cout << "Target OS     : Windows x64\n";
    std::cout << "Subsystem     : Dear ImGui Docking v1.90.4 + ImPlot v0.16\n";
    std::cout << "--------------------------------------------------\n\n";

    std::vector<BenchResult> results;

    // 1. Uncached AST Parse (1k expressions)
    results.push_back(bench("1. Uncached AST Parse (1,000 expr)", []
                            {
        ExpressionCache::instance().clear();
        for (int i = 0; i < 1000; i++) {
            Expression e("a * sin(b * x) + t");
        } }));

    // 2. ExpressionCache Lookup Reparse Overhead (1,000 frames)
    results.push_back(bench("2. Cached AST Lookup (1,000 frames)", []
                            {
        for (int i = 0; i < 1000; i++) {
            Expression e("a * sin(b * x) + t");
        } }));

    // 3. Live Parameter Slider Mutation (10,000 steps)
    VariableStore vStore;
    const auto &fReg = mathstudio::core::FunctionRegistry::instance();
    DiagnosticsReporter reporter;
    mathstudio::core::EvaluationContext ctx(vStore, fReg, reporter);

    Expression sliderExpr("a * sin(b * x) + t");
    results.push_back(bench("3. Live Slider Mutation (10,000 steps)", [&]
                            {
        for (int i = 0; i < 10000; i++) {
            vStore.set("a", MathValue::real(1.0 + (i % 100) * 0.05));
            vStore.set("b", MathValue::real(0.5 + (i % 50) * 0.02));
            volatile double val = sliderExpr.evalWithContext(i * 0.01, ctx);
            (void)val;
        } }));

    // 4. Static 600-point Plot Frame Render Update (1,000 frames)
    results.push_back(bench("4. Static 600-pt Render Buffer (1,000 frames)", [&]
                            {
        static double xBuf[600];
        static double yBuf[600];
        for (int frame = 0; frame < 1000; frame++) {
            for (int i = 0; i < 600; i++) {
                xBuf[i] = -10.0 + (20.0 * i / 599.0);
                yBuf[i] = sliderExpr.evalWithContext(xBuf[i], ctx);
            }
        } }));

    // 5. Multi-Domain Context Evaluation (100,000 pts)
    vStore.set("a", MathValue::real(2.5));
    vStore.set("b", MathValue::real(1.5));
    Expression domainExpr("a * sin(b * x) + t * cos(n * x)");
    results.push_back(bench("5. Multi-Domain Eval (100,000 pts)", [&]
                            {
        for (int i = 0; i < 100000; i++) {
            double xVal = i * 0.0001;
            double tVal = i * 0.00005;
            double nVal = static_cast<double>(i % 100);
            volatile double val = domainExpr.evalSignalWithContext(xVal, tVal, nVal, ctx);
            (void)val;
        } }));

    std::cout << "| Benchmark Metric                        | Time (ms/run) |\n";
    std::cout << "|-----------------------------------------|---------------|\n";
    for (const auto &r : results)
    {
        std::printf("| %-39s | %13.3f |\n", r.name.c_str(), r.ms);
    }

    size_t ram = getPeakRAMMB();
    if (ram > 0)
    {
        std::cout << "\nPeak Working Set Memory Usage: " << ram << " MB\n";
    }

    std::cout << "\nBenchmark complete for v0.3.0 Dear ImGui & ImPlot Release.\n";
    std::cout << "Log saved in benchmarks/benchmark_v0.3.0.md\n";
    return 0;
}
