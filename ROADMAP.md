# MathStudio Roadmap

## Release Progression Strategy

```mermaid
graph TD
    v001["v0.0.1<br/>• Prototype Release"] --> v010
    v010["v0.1.0<br/>• Foundation Release (Current)<br/>• Project Structure & Refactoring<br/>• 132 Tests & Benchmark Suite<br/>• Native Windowing & Dark Palette<br/>• Scientific Notation & Signal Vars (t, n)<br/>• Extrema Labels & Classification"] --> v020
    v020["v0.2.0<br/>• Architecture Rewrite<br/>• AST Tree & MathValue Substrate<br/>• Visitor Pattern (EvalVisitor, PrintVisitor)<br/>• Dear ImGui Docking UI & ImPlot<br/>• Plugin Architecture"] --> v030
    v030["v0.3.0<br/>• Calculus Engine<br/>• Symbolic Derivatives & Taylor Series<br/>• Numerical Integration & Limits"] --> v040
    v040["v0.4.0<br/>• Signal Processing (DSP)<br/>• FFT, DFT, IFFT & Convolution<br/>• Filter Design & Aliasing Demos"] --> v050
    v050["v0.5.0<br/>• Linear Algebra Engine<br/>• Matrix & Vector Operations"] --> v060
    v060["v0.6.0<br/>• Computer Algebra System (CAS)"] --> v100
    v100["v1.0.0<br/>★ Stable Scientific Computing Engine"]

    style v010 fill:#e1f5fe,stroke:#01579b
    style v020 fill:#fff3e0,stroke:#ef6c00
    style v030 fill:#f3e5f5,stroke:#7b1fa2
    style v040 fill:#e8f5e9,stroke:#2e7d32
    style v100 fill:#fff9c4,stroke:#fbc02d,stroke-width:2px
```

---

## Version Tracker Table

| Version | Focus | Status | Description |
|---------|-------|--------|-------------|
| **v0.0.1** | Prototype | ✅ Released | Initial proof-of-concept plotter with RPN evaluator |
| **v0.1.0** | Foundation | ✅ Complete | Refactored architecture, 132 unit tests, benchmark suite, native dark titlebar, extrema labels, scientific notation |
| **v0.2.0** | Architecture Rewrite | 📋 Planned | AST tree hierarchy, MathValue type system, Visitor pattern, Dear ImGui docking UI |
| **v0.3.0** | Calculus | 📋 Planned | Symbolic differentiation, numerical integration, limits, Taylor series |
| **v0.4.0** | Signal Processing | 📋 Planned | FFT, DFT, IFFT, convolution, filter response visualizer |
| **v0.5.0** | Linear Algebra | 📋 Planned | Matrix & vector editor, determinants, eigenvalues |
| **v0.6.0** | Computer Algebra | 📋 Planned | CAS expansion, symbolic simplification, expression factoring |
| **v1.0.0** | Major Milestone | 🎯 Goal | **Stable Scientific Computing Engine** |

---

## Guiding Principles

1. **Semantic Versioning Integrity**: `v0.1.0` establishes the stable foundation. Major `v1.0.0` is earned when the full AST + ImGui + Calculus + DSP suite is complete.
2. **Measurable Performance Progress**: Every release tracks performance metrics against baseline benchmarks.
3. **Architecture Discipline**: No new domain features until the `v0.2.0` AST + MathValue substrate is locked.
