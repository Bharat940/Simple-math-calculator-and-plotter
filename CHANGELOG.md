# Changelog
All notable changes to MathStudio will be documented in this file.
Format based on [Keep a Changelog](https://keepachangelog.com).

## [Unreleased]

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
