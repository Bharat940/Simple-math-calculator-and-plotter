# Contributing to MathStudio

Thank you for your interest in contributing to MathStudio!

## Project Overview
MathStudio is a modern mathematical computation and visualization tool built in C++17 with SDL2 and CMake. See `docs/architecture.md` for a full structural overview.

## Branching Strategy
- `main` — Stable release branch. All release tags (`v1.0.0`, `v1.1.0`) originate here.
- `v2-engine-rewrite` — Architecture rewrite branch for v2.0 (AST, MathValue, Dear ImGui).
- `feature/*` or `fix/*` — Feature and fix branches created off `main` or active development branches.

## Building the Project

```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build targets
cmake --build build
```

## Running Unit Tests

```bash
# Run unit test executable
./build/tests
```

## Commit Message Format

Use standardized commit tags:
- `[v1.1] Fix: parse scientific notation in tokenizer`
- `[v1.1] Add: tau constant and t/n variables`
- `[v1.1] Docs: update README with architectural roadmap`
- `[v2.0] Add: ASTBuilder node creation logic`
