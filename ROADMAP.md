# MathStudio Master Architecture Roadmap (v0.1.0 → v1.0.0)

**Project Goal**: Evolving MathStudio from a simple function plotter into a modern, lightweight, high-performance **scientific computing engine** — combining the interactive visualization of Desmos with the symbolic capabilities of MATLAB / SymPy and the parsing elegance of modern compiler frontends.

---

## 1. Core Architecture Pipeline

The central representation of expressions in MathStudio is a **pure Abstract Syntax Tree (AST)** driven by a Pratt Parser. The legacy RPN postfix interpreter has been deprecated into `legacy/` and replaced by a pure compiler pipeline:

```mermaid
graph TD
    Input["Expression String"] --> Cache{"ExpressionCache"}
    Cache -- Hit --> AST["AST (std::unique_ptr<ASTNode>)"]
    Cache -- Miss --> Lexer["src/compiler/lexer/ (Tokenizer)"]
    
    Lexer --> Pratt["src/compiler/parser/ (Pratt Parser)"]
    Pratt --> Diag["src/compiler/diagnostics/"]
    Diag --> AST
    
    AST --> PassMgr["src/compiler/passes/ (PassManager)"]
    
    PassMgr --> FoldPass["ConstantFoldVisitor (v0.2.0)"]
    PassMgr --> SimpPass["SimplifyVisitor (v0.2.0)"]
    PassMgr --> OptPass["Optimization Passes"]
    
    PassMgr --> EvalVis["EvalVisitor (O(1) Slots + Enum Opcodes)"]
    PassMgr --> DerivVis["DerivativeVisitor (v0.4.0)"]
    PassMgr --> PrintVis["PrintVisitor"]

    EvalVis --> Context["EvaluationContext"]
    Context --> MVal["MathValue (C++17 std::variant)"]
```

---

## 2. Master Version Track & Milestones

```mermaid
graph TD
    v010["v0.1.0 Foundation Release (Complete)<br/>• Refactored C++17 Pipeline<br/>• 132 Unit Tests & Benchmark Suite<br/>• Native Windows Dark Titlebar<br/>• Extrema Canvas Labels & Signal Variables (t, n)"] --> v020

    v020["v0.2.0 AST Engine Architecture (Complete & Frozen)<br/>• Compiler Substructure (src/compiler/)<br/>• Pratt Parser Engine (Binding-Power Operator Table)<br/>• MathValue C++17 std::variant (double, complex, Matrix, Vector, bool)<br/>• EvaluationContext (Vars, Functions, Diagnostics)<br/>• Visitor Substrate (EvalVisitor, PrintVisitor)<br/>• PassManager Passes (ConstantFoldVisitor, SimplifyVisitor)<br/>• O(1) Variable Slots & Enum Opcodes (BinaryOpType, UnaryOpType)<br/>• ExpressionCache & Deprecate RPN to legacy/"] --> v030

    v030["v0.3.0 Dear ImGui & ImPlot UI Overhaul (Complete & Frozen)<br/>• Dear ImGui Docking UI Subsystem<br/>• ImPlot Multi-Domain Canvas (Cartesian, Time, Discrete, FFT)<br/>• Variable Inspector (sliders, Play/Pause ▶/⏸, --var CLI sync, trash 🗑 delete)<br/>• Real-time Derivative Curves [D] & Multi-domain Solvers<br/>• --check-leaks MSVC CRT Heap Audit (0 Leaks)<br/>• Zero-Allocation Plot Render Buffers"] --> v031

    v031["v0.3.1 Performance & Profiling Baseline (Current Focus)<br/>• 4-Tier Benchmark Taxonomy (engine, rendering, ui, integration)<br/>• Zero-Heap FrameProfiler with Fixed Zone Enums & Scope Timers<br/>• Frame Time Percentiles (Min, Med, P95, P99, Max Distribution)<br/>• Viewport State Hashing & Dirty Flag Resampling (Zero Idle Eval)<br/>• Machine-Readable Telemetry (benchmarks/results/*.json)<br/>• Platform Profiler Playbooks (VS CPU Profiler, WPR/WPA, perf)"] --> v040

    v040["v0.4.0 Calculus Engine (Next)<br/>• Symbolic Differentiation (DerivativeVisitor)<br/>• Numerical Integration (Simpson's 3/8, Adaptive Quadrature)<br/>• Limits & Taylor Series Expansion<br/>• ASTArenaAllocator Pool Memory"] --> v050

    v050["v0.5.0 Signal Processing (DSP)<br/>• DFT → FFT → IFFT → Discrete Convolution Engine<br/>• Correlation, Window Functions (Hann, Hamming, Blackman)<br/>• Spectrogram & Dual-Domain Canvas<br/>• SIMD (AVX2) Vectorization"] --> v060

    v060["v0.6.0 Linear Algebra Engine<br/>• Typed Matrix Operations (Matrix<T>)<br/>• Determinants, Inverses, Eigenvalues"] --> v070

    v070["v0.7.0 Computer Algebra System (CAS)<br/>• Full CAS Symbolic Engine<br/>• Polynomial Expansion & Factoring"] --> v080

    v080["v0.8.0 Sessions & Project Serialization<br/>• AST JSON Serialization<br/>• Multi-Level Undo / Redo & Session State Manager"] --> v085

    v085["v0.8.5 Renderer Abstraction Layer<br/>• IRenderer Hardware Abstraction Interface<br/>• Modular Backends: SDL_Renderer | OpenGL | Vulkan | DirectX"] --> v100

    v100["v1.0.0 ★ Stable Scientific Computing Engine<br/>• Production Hardened Release"]

    style v010 fill:#e1f5fe,stroke:#01579b
    style v020 fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style v030 fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style v031 fill:#fff3e0,stroke:#ef6c00,stroke-width:3px
    style v040 fill:#f3e5f5,stroke:#7b1fa2
    style v100 fill:#fff9c4,stroke:#fbc02d,stroke-width:2px
```

---

## 3. Version Specifications

| Version | Focus | Status | Key Deliverables & Architectural Specs |
|---------|-------|--------|---------------------------------------|
| **v0.1.0** | Foundation | ✅ Complete | C++17 refactoring, 139 unit tests, benchmark baseline, dark titlebar, coordinate labels |
| **v0.2.0** | AST Engine Architecture | ✅ Complete & Frozen | `src/compiler/`, Pratt parser, `std::variant<MathValue>`, `EvaluationContext`, `ExpressionCache`, `ConstantFoldVisitor`, `SimplifyVisitor`, $O(1)$ variable slots, `BinaryOpType`/`UnaryOpType` enums |
| **v0.3.0** | ImGui & ImPlot UI Overhaul | ✅ Complete & Frozen | Dear ImGui docking UI, Render-to-Texture `CanvasPanel`, ImPlot multi-domain visualizers ($f(x)$, $f(t)$, $f[n]$, FFT), `src/ui/widgets/` layer, Variable Inspector with trash `🗑` delete & `--var` CLI sync, `--check-leaks` MSVC CRT audit, zero-allocation buffers, real-time derivative curves `[D]` |
| **v0.3.1** | Performance & Profiling Baseline | 🚀 Active | 4-tier benchmark taxonomy (`engine/`, `rendering/`, `ui/`, `integration/`), zero-heap `FrameProfiler` with fixed zone enums, frame-time latency percentiles (P50/P95/P99), viewport dirty flag caching (zero idle re-evaluations), machine-readable JSON telemetry |
| **v0.4.0** | Calculus Engine | 📋 Next | Symbolic differentiation (`DerivativeVisitor`), numerical integration (Simpson's 3/8, Adaptive Quadrature), Taylor series, `ASTArenaAllocator`, adaptive sub-pixel resampling |
| **v0.5.0** | DSP & Signals | 📋 Planned | DFT, FFT, IFFT, discrete convolution, correlation, window functions, spectrogram visualization, SIMD (AVX2) vectorization |
| **v0.6.0** | Linear Algebra | 📋 Planned | Matrix & Vector AST nodes, typed `Matrix<T>`, Gaussian elimination, determinants, eigenvalues |
| **v0.7.0** | CAS Engine | 📋 Planned | Symbolic computer algebra system, polynomial expansion, factoring, symbolic simplification |
| **v0.8.0** | Sessions & Serialization | 📋 Planned | AST JSON serialization, workspace project files, undo/redo manager |
| **v0.8.5** | Renderer Abstraction | 📋 Planned | `IRenderer` interface decoupling renderer backends (`SDL_Renderer`, `OpenGL`, `Vulkan`, `DirectX`) |
| **v1.0.0** | Major Milestone | 🎯 Goal | **Stable Scientific Computing Engine** |

---

## 4. Architectural Module Structure

| Module Layer | Location | Key Responsibilities |
|--------------|----------|----------------------|
| **Core Storage** | `src/core/` | C++17 `std::variant<MathValue>`, `VariableStore`, `FunctionRegistry`, `EvaluationContext`, `ExpressionCache` |
| **Compiler Substructure** | `src/compiler/` | Pratt Parser (`PrattParser.hpp`), AST Tree (`Node.hpp` with `unique_ptr`), Visitors (`EvalVisitor`, `PrintVisitor`), Passes (`PassManager`, `ConstantFoldVisitor`), Diagnostics (`Diagnostics.hpp`) |
| **Math Pipeline** | `src/math/` | High-level `Expression` wrapper, `tokenizer`, `solver` (Newton-Raphson/Bisection), `numerical` (derivatives/tangents), `constants_registry` |
| **Legacy RPN** | `legacy/` | Historical `shunting_yard.h/cpp` reference (marked `[[deprecated]]`) |
| **UI & Renderer** | `src/ui/` & `src/renderer` | SDL2 canvas plotter, Dear ImGui control panels, ImPlot visualizers, Obsidian dark theme |

---

## 5. Performance Engineering Strategy

1. **Pratt Parser Efficiency**: Eliminates operator stack shuffling, yielding **~4.1x faster uncached parsing** (23.33 ms vs 96.08 ms).
2. **ExpressionCache Zero-Overhead**: Eliminates re-parsing on render loops (0.00 ms re-parse overhead).
3. **AST as the Enabler for Passes**: The AST enables `ConstantFoldVisitor` (`2*pi*x -> 6.283185*x`), `SimplifyVisitor` (`x*1 -> x`), $O(1)$ variable array indexing, and AST arena memory allocation.

---

## 6. Comprehensive Optimization Strategy & Risk Matrix

| Optimization | Description / Scope | Risk Profile | Implementation Target |
| :--- | :--- | :---: | :--- |
| **1. Zero-Allocation Plot Buffers** | Pre-allocates static sample vectors for 600 plot points per frame, eliminating frame-by-frame heap allocations. | **0% Risk** (Safe) | **v0.3.0 Complete** (Implemented & Verified) |
| **2. `ASTArenaAllocator`** | Pre-allocates a 64 KB pool chunk for AST nodes, avoiding heap fragmentation and maximizing CPU L1 cache hits. | **Low Risk** | **v0.4.0 (Calculus Engine)** — Pairs with `DerivativeVisitor` |
| **3. SIMD (AVX2 / SSE2) Vectorization** | Uses 256-bit AVX instructions (`_mm256_add_pd`) to evaluate 4 plot points per CPU cycle. | **Medium Risk** (Needs Fallback) | **v0.5.0 (DSP Engine)** — Critical for 4096-point FFT buffers |
| **4. Adaptive Sub-Pixel Resampling** | Samples more points on high-curvature regions and fewer points on flat lines. | **Low Risk** | **v0.4.0 (Calculus Engine)** — Uses second derivative $f''(x)$ |

