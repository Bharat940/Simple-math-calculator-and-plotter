# MathStudio Master Architecture

> **Current Architecture: v0.3.0**  
> MathStudio is a high-performance scientific computing and interactive mathematical visualization engine built in C++17.

---

## 1. Architectural Overview & System Layers

MathStudio is organized into distinct layers where the math compiler, core engine, immediate-mode GUI, and CLI operate independently:

```mermaid
graph TD
    subgraph UI ["Immediate-Mode UI Layer (src/ui/)"]
        ImGuiDock["Dear ImGui Docking Workspace (UIManager)"]
        ImPlotCanvas["ImPlot Multi-Domain Canvas (PlotPanel)<br/>Cartesian f(x) | Time f(t) | Discrete f[n] | FFT Spectrogram"]
        Panels["Dockable Panels & Widgets<br/>ControlPanel | VariableInspectorPanel | PerformanceProfilerPanel | DiagnosticsConsolePanel"]
    end

    subgraph App ["Application & CLI Layer (src/app/, src/cli/)"]
        AppLifecycle["Application (SDL2 Window & ImGui Lifecycle)"]
        CliRunner["CliRunner (Batch Evaluation -e, -s, -d, -i)"]
        CliParser["CliParser (Argument Parsing & Whitelist Validation)"]
    end

    subgraph Compiler ["Compiler Pipeline (src/compiler/)"]
        Tokenizer["Tokenizer / Lexer"]
        Pratt["Pratt Parser (Binding-Power Operator Table)"]
        ASTTree["Abstract Syntax Tree (unique_ptr<ASTNode>)"]
        PassMgr["PassManager"]
        PassFold["ConstantFoldVisitor (2*pi -> 6.283185)"]
        PassSimp["SimplifyVisitor (x*1 -> x, sin(0) -> 0)"]
        Visitors["Visitor Substrate (EvalVisitor, PrintVisitor)"]
        Diagnostics["DiagnosticsReporter"]
    end

    subgraph Core ["Core Data Model (src/core/)"]
        MathVal["MathValue (C++17 std::variant<double, complex, Matrix, Vector, bool>)"]
        VarStore["VariableStore (O(1) Direct Memory Slots)"]
        FuncReg["FunctionRegistry (Thread-Safe Mathematical Builtins)"]
        EvalCtx["EvaluationContext"]
        ExprCache["ExpressionCache (AST String Hashing)"]
        EngineProf["EngineProfiler (Latency & Cache Telemetry)"]
    end

    AppLifecycle --> ImGuiDock
    ImGuiDock --> ImPlotCanvas
    ImGuiDock --> Panels
    Panels --> EvalCtx
    ImPlotCanvas --> EvalCtx

    CliRunner --> EvalCtx
    EvalCtx --> ExprCache
    ExprCache -- Miss --> Tokenizer
    Tokenizer --> Pratt
    Pratt --> ASTTree
    ASTTree --> PassMgr
    PassMgr --> PassFold
    PassMgr --> PassSimp
    PassMgr --> Visitors
    Visitors --> EvalCtx
    EvalCtx --> VarStore
    EvalCtx --> FuncReg
    Visitors --> MathVal
```

---

## 2. Compiler Pipeline (`src/compiler/`)

Expressions are compiled into an Abstract Syntax Tree using a Pratt Parser and evaluated via AST visitor passes:

```mermaid
graph LR
    Input["Expression String"] --> Lexer["Tokenizer"]
    Lexer --> Pratt["PrattParser"]
    Pratt --> AST["AST Node Hierarchy"]
    AST --> PassManager["PassManager Pipeline"]
    
    PassManager --> Fold["ConstantFoldVisitor"]
    PassManager --> Simplify["SimplifyVisitor"]
    PassManager --> Eval["EvalVisitor"]
    
    Eval --> Result["MathValue Result"]
```

### Implemented Compiler Components
- **Pratt Parser (`src/compiler/parser/PrattParser.hpp`)**: Precedence and associativity parsing via binding-power lookup tables.
- **AST Nodes (`src/compiler/ast/Node.hpp`)**: Memory-efficient nodes with 1-byte opcode enums (`BinaryOpType`, `UnaryOpType`).
- **Pass Manager (`src/compiler/passes/PassManager.hpp`)**:
  - `ConstantFoldVisitor`: Parse-time constant evaluation (e.g. `2*pi` $\rightarrow$ `6.283185`).
  - `SimplifyVisitor`: Algebraic identity reduction (`x+0` $\rightarrow$ `x`, `x*1` $\rightarrow$ `x`, `x*0` $\rightarrow$ `0`, `sin(0)` $\rightarrow$ `0`, `cos(0)` $\rightarrow$ `1`).
- **Evaluation Visitor (`src/compiler/visitors/EvalVisitor.hpp`)**: AST tree walker returning C++17 `MathValue`.
- **Diagnostics (`src/compiler/diagnostics/Diagnostics.hpp`)**: Error location, source ranges, and formatting.

---

## 3. Core Engine Layer (`src/core/`)

- **`MathValue`**: C++17 `std::variant<double, std::complex<double>, Matrix, Vector, bool>` providing type safety and algebraic operations.
- **`VariableStore`**: Direct $O(1)$ indexed memory slots for high-frequency variables (`x`, `t`, `n`, `theta`, `z`, `ans`), eliminating string hash map overhead.
- **`FunctionRegistry`**: Mathematical function dispatch with domain validation.
- **`ExpressionCache`**: Thread-safe AST cache indexed by expression string key.
- **`EngineProfiler`**: Real-time telemetry tracking parse latency, evaluation rate, and cache hit ratio.

---

## 4. Immediate-Mode UI Architecture (`src/ui/`)

- **`UIManager`**: Docking manager coordinating the Dear ImGui docking space and panel layout.
- **`PlotPanel`**: ImPlot multi-domain canvas managing:
  - **Cartesian $f(x)$**: Multi-curve continuous function plotting.
  - **Time Domain $f(t)$**: Signal waveform plotting.
  - **Discrete Series $f[n]$**: Discrete stem sequence plotting.
  - **FFT Spectrogram**: Real-time Power Spectral Density $|X(f)|^2$ analyzer.
  - **Zero-Allocation Plot Buffers**: Pre-allocated static 600-point arrays for 60+ FPS rendering without frame-by-frame heap allocations.
  - **Derivative Overlay `[D]`**: Toggleable real-time derivative curves ($y = f'(x)$, signal velocity $\frac{df}{dt}$, discrete difference $\Delta f[n]$).
- **`VariableInspectorPanel`**: Parameter sliders with Play ▶ / Pause ⏸ animation controls and `--var` CLI synchronization.
- **`PerformanceProfilerPanel`**: Real-time parse latency, evaluation rate, cache hit ratio, and RAM meters.
- **`DiagnosticsConsolePanel`**: Live token and parse error reporting.

---

## 5. Application & CLI Layer (`src/app/`, `src/cli/`)

- **`Application` (`src/app/Application.hpp`)**: SDL2 window creation, Windows DWM dark titlebar integration, ImGui/ImPlot context setup, and main rendering loop.
- **`CliParser` (`src/cli/CliParser.hpp`)**: Parses command line arguments (`-e`, `-s`, `-d`, `-i`, `--var`, `--check-leaks`, `--benchmark`).
- **`CliRunner` (`src/cli/CliRunner.hpp`)**: Headless batch execution for mathematical evaluation, root solving, intersection finding, and CRT memory leak audits.

---

## 6. Historical Architecture Deprecation

```
Legacy Architecture (v0.0.1 – v0.1.x) [DEPRECATED & FROZEN IN legacy/]
  Shunting-Yard Parser ──► Postfix RPN Vector ──► Postfix Stack Evaluator
  (Preserved exclusively in legacy/shunting_yard.cpp for historical benchmark regression testing)

Current Architecture (v0.2.0 – v0.3.0) [BUILT & VERIFIED]
  Pratt Parser ──► AST Nodes ──► PassManager (ConstantFold + Simplify) ──► EvalVisitor ──► MathValue
  + Dear ImGui Docking UI & ImPlot Multi-Domain Canvas
  + Zero-Allocation 600-Point Plot Buffers
  + MSVC CRT / Valgrind Memory Leak Audit (--check-leaks)
```
