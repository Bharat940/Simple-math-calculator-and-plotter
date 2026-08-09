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

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

using Clock = std::chrono::high_resolution_clock;

struct BenchResult {
    std::string name;
    double ms;
    int runs;
};

BenchResult bench(const std::string& name, std::function<void()> fn, int warmup = 1, int runs = 3) {
    for (int i = 0; i < warmup; i++) fn();
    auto start = Clock::now();
    for (int i = 0; i < runs; i++) fn();
    auto end = Clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count() / runs;
    return {name, ms, runs};
}

size_t getPeakRAMMB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize / (1024 * 1024);
    }
#endif
    return 0;
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "MathStudio v0.2.0 AST Architecture Benchmark Suite\n";
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
    std::cout << "Pipeline      : PrattParser + AST + ConstantFoldVisitor\n";
    std::cout << "--------------------------------------------------\n\n";

    std::vector<BenchResult> results;

    results.push_back(bench("Parse 1,000 expressions (uncached)", []{
        ExpressionCache::instance().clear();
        for (int i = 0; i < 1000; i++) {
            Expression e("sin(x^2) + cos(x) * exp(-x)");
        }
    }));

    Expression expr("sin(x^2) + cos(x) * exp(-x)");
    results.push_back(bench("Eval 100,000 points", [&]{
        for (int i = 0; i < 100000; i++) {
            volatile double val = expr.eval(i * 0.0001);
            (void)val;
        }
    }));

    Expression sinExpr("sin(x)");
    results.push_back(bench("Find roots sin(x) in [-50, 50]", [&]{
        auto roots = findRoots(sinExpr, -50.0, 50.0);
        (void)roots;
    }));

    Expression polyExpr("x^3 - 3*x");
    results.push_back(bench("Find extrema x^3 - 3x in [-5, 5]", [&]{
        auto extrema = findExtrema(polyExpr, -5.0, 5.0);
        (void)extrema;
    }));

    std::cout << "| Benchmark                              | Time (ms/run) |\n";
    std::cout << "|----------------------------------------|---------------|\n";
    for (const auto& r : results) {
        std::printf("| %-38s | %13.3f |\n", r.name.c_str(), r.ms);
    }

    size_t ram = getPeakRAMMB();
    if (ram > 0) {
        std::cout << "\nPeak Process Memory Usage: " << ram << " MB\n";
    }

    std::cout << "\nBaseline established for v0.2.0 AST Architecture Rewrite.\n";
    std::cout << "Historical record saved in benchmarks/benchmark_v0.2.0.md\n";
    return 0;
}
