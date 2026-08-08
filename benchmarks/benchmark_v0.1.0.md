# MathStudio Performance Baseline — v0.1.0 (Foundation Release)

**Date**: 2026-08-08  
**Commit Baseline**: Foundation Release (v0.1.0 Tag)  
**System & Build Environment**:
- **CPU**: AMD Ryzen 5 7520U with Radeon Graphics
- **Operating System**: Windows 11 x64 (Build 22631)
- **Compiler**: MSVC 19.51 (Visual Studio 2026 Developer Toolchain v18.8.0)
- **Build Configuration**: `CMAKE_BUILD_TYPE=Release` (`/O2` optimization)
- **Build Generator**: NMake / CMake 3.31
- **Target Architecture**: amd64 (x64)

---

## Executable & Memory Metrics

| Asset / Process | Size / Working Set | Notes |
|-----------------|--------------------|-------|
| `mathstudio.exe` | **1.12 MB** | Main SDL2 GUI + CLI binary |
| `tests.exe` | **0.45 MB** | 132 passing unit tests binary |
| `benchmarks.exe` | **0.37 MB** | Benchmark suite binary |
| **Engine Peak Working Set** | **4.0 MB** | Working set during 100,000 evaluations |
| **GUI Idle Working Set** | **~18.5 MB** | Idle SDL2 window with canvas framebuffer |
| **GUI Peak Working Set** | **~24.2 MB** | Peak memory with 5 simultaneous functions rendered |

---

## Performance Benchmark Measurements

| Benchmark Scenario | Input / Expression | Iterations | Average Time (ms/run) | Time per unit |
|--------------------|--------------------|------------|-----------------------|---------------|
| **Parser** | `sin(x^2) + cos(x) * exp(-x)` | 1,000 expressions | **96.08 ms** | ~96.08 μs / expr |
| **Evaluator** | `sin(x^2) + cos(x) * exp(-x)` | 100,000 samples | **1329.58 ms** | ~13.29 μs / point |
| **Root Solver** | `sin(x)` in `[-50, 50]` | 50 roots scanned | **7.33 ms** | ~0.14 ms / root |
| **Extrema Solver** | `x^3 - 3*x` in `[-5, 5]` | 2 extrema classified | **1.68 ms** | ~0.84 ms / extremum |

---

## Architectural Notes & Optimization Goals for v0.2.0 (AST Rewrite)

1. **Parser Overhead**: Infix tokenizer + Shunting-Yard RPN currently re-parses expressions on every frame unless cached. `v0.2.0` AST will parse **once** into an expression tree.
2. **Evaluation Optimization**: The AST architecture enables expression caching, constant folding, tree simplification, and future multithreaded evaluation. These optimizations are expected to significantly reduce evaluation time compared to the current RPN implementation.
3. **Memory Footprint**: Current 4.0 MB Engine Peak Working Set establishes a tight baseline for comparing the AST node allocation overhead in `v0.2.0`.
