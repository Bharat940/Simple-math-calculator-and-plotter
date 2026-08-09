# Changelog
All notable changes to MathStudio will be documented in this file.
Format based on [Keep a Changelog](https://keepachangelog.com).

## [Unreleased]

## [v0.2.0] — AST Compiler Architecture Release - 2026-08-09

### Added
- **Compiler Substructure (`src/compiler/`)**: Decoupled compiler frontend (`ast`, `parser`, `visitors`, `passes`, `diagnostics`) from domain math engine.
- **Pratt Parsing Engine**: Implemented precedence and operator associativity via binding-power operator tables in `PrattParser`.
- **C++17 `MathValue` Variant**: Powered by `std::variant<double, std::complex<double>, Matrix, Vector, bool>` for type-safe compile-time operations.
- **Compiler Pass Pipeline**:
  - `ConstantFoldVisitor`: Parse-time evaluation of constant expressions (e.g. `2*pi → 6.283185`).
  - `SimplifyVisitor`: Parse-time algebraic identity reduction (`x+0 → x`, `x*1 → x`, `x*0 → 0`, `pow(x,1) → x`, `sin(0) → 0`, `cos(0) → 1`).
- **O(1) Direct Variable Slots**: Accelerated `VariableStore` with direct slots (`xVal`, `tVal`, `nVal`, `thetaVal`, `zVal`, `ansVal`) bypassing string hash map lookups.
- **Compact Enum Opcodes**: Introduced `BinaryOpType` and `UnaryOpType` enums, removing 32-byte `std::string op` heap overhead per node.
- **Signal Variable Binding**: Automatic binding of `t`, `n`, `theta`, and `z` to `x` in 2D plot canvas evaluations (`sin(2*pi*t)`).
- **Diagnostics Reporter**: Position-aware error reporting providing caret indicators and error messages.
- **Legacy Deprecation**: Deprecated RPN shunting yard into `legacy/shunting_yard.h/cpp` for historical benchmarking.

### Performance
- **Pure Release (`/O2`) Performance**:
  - **Parsing**: 0.806 ms / 1,000 expressions (comparable / slightly faster than v0.1.0)
  - **Evaluation**: 193.91 ms / 100,000 points (~1.93 μs / point)
  - **Memory Footprint**: 4.0 MB Peak RAM

---

## [v0.1.0] — Foundation Release - 2026-08-08

### Added
- **Signal & Discrete Variables**: Native parsing for `t` (time domain) and `n` (discrete sequence index)
- **Scientific Notation**: Full parsing support for numbers like `1e-5`, `2.3e10`, `3E2`
- **Constants**: Added `tau` (`= 2*pi`)
- **Canvas Text Callouts**: Formatted `x=...` and `(x, y)` coordinate labels rendered directly on graph markers
- **Extrema Classification**: Classifies points into local minima, local maxima, and saddle points using numerical second derivatives
- **Benchmark Suite**: Added `benchmarks/bench_v1.cpp` establishing baseline metrics (parsing, eval, roots, extrema)
- **Documentation & Diagrams**: Added `CHANGELOG.md`, `ROADMAP.md`, `CONTRIBUTING.md`, and architecture diagrams in `docs/`

### Fixed
- **Windows High-DPI & Titlebar**: Handled High-DPI scaling (`SDL_HINT_WINDOWS_DPI_AWARENESS`) and enabled native Windows Dark Mode Titlebar (`DwmSetWindowAttribute`)
- **Window Resizing & Fullscreen**: Added `SDL_WINDOW_RESIZABLE` 1280x720 window with `F11` fullscreen toggle
- **Windows Macro Collision**: Fixed `<windows.h>` `min`/`max` macro collision with `NOMINMAX` guard

### Changed
- **Deep Slate Obsidian Dark Theme**: Modernized canvas background (`#14161c`) and grid (`#262a36`)
- **Testing**: Expanded unit test suite to **132/132 passing tests**

### Performance Baseline
- **Parser**: ~96.0 ms / 1,000 expressions (~96 μs / expression)
- **Evaluator**: ~1.21 s / 100,000 samples (~12 μs / sample)
- **Root Solver**: ~7.20 ms (`sin(x)` in `[-50, 50]`)
- **Extrema Solver**: ~1.36 ms (`x^3 - 3x` in `[-5, 5]`)

---

## [v0.0.1] — Prototype Release - 2026-04-23
### Added
- Initial prototype release: basic function plotter with RPN expression parsing
- 18 built-in math functions with domain validation
- Constants: π, e, φ
- Root finding (bisection + Newton-Raphson), intersection detection, extrema
- SDL2 GUI: adaptive curve rendering, zoom/pan, tangent lines, multi-function legend
- CLI modes: `-e` (evaluate), `-s` (solve), `-i` (intersections)
- 119 unit tests, CI/CD on Linux/macOS/Windows
