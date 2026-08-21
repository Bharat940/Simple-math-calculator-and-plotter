# MathStudio Performance Profiling & Diagnostic Guide

> **Target Version: v0.3.1+**  
> This guide outlines the official workflows for profiling MathStudio across Windows, Linux, and macOS using both internal instrumentation and external sampling profilers.

---

## 1. Profiling Architecture Overview

MathStudio uses two complementary profiling mechanisms:

1. **Application-Level Instrumentation (`FrameProfiler`)**:
   - High-resolution zone timing around discrete engine and UI subsystems (`Evaluation`, `Sampling`, `ImGui`, `ImPlot`, `SDL`).
   - Run inside the application with zero external tooling required.
2. **External Sampling Profilers**:
   - Statistical program counter sampling identifying exact CPU hot paths, caller/callee trees, and flame graphs.

---

## 2. Windows Profiling Playbook (PowerShell Native)

### 2.1 Internal Instrumentation (No Visual Studio Required)

Compile the optimized Release build:

```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Run MathStudio normally with real-time HUD telemetry:

```powershell
.\build\Release\mathstudio.exe "sin(x^2) + cos(x^3) * exp(-x^2)"
```

The **Performance Profiler** panel displays live zone breakdowns and P50/P95/P99 latency distribution.

---

### 2.2 System-Level Tracing with Windows Performance Recorder (WPR)

To record ETW system traces directly from PowerShell without GUI overhead:

```powershell
# 1. Start profiling trace
wpr -start GeneralProfile -start CPU

# 2. Run target controlled scenario (e.g. 10s heavy multi-curve plotting)
.\build\Release\mathstudio.exe "sin(x^2) + cos(x^3) * exp(-x^2)"

# 3. Stop trace and save to ETL file
wpr -stop mathstudio_trace.etl

# 4. Open trace in Windows Performance Analyzer (WPA)
wpa mathstudio_trace.etl
```

---

### 2.3 Visual Studio CPU Usage Profiler (`Alt + F2`)

If using the Visual Studio IDE:
1. Open MathStudio in Visual Studio.
2. Navigate to **Debug** $\rightarrow$ **Performance Profiler** (`Alt + F2`).
3. Check **CPU Usage** and ensure build configuration is set to **Release**.
4. Click **Start** and run Scenario C or D for 20 seconds.
5. Inspect the **Call Tree**, **Hot Path**, and **Flame Graph**.

---

## 3. Linux Profiling Playbook (`perf`)

### 3.1 Recording CPU Call Graphs

```bash
# Build optimized binary with debug symbols
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build

# Record CPU samples at 99 Hz with call stacks
perf record -F 99 -g ./build/mathstudio "sin(x^2) + cos(x^3)"

# Inspect interactive call tree in terminal
perf report
```

### 3.2 Generating Flame Graphs

```bash
# Export perf script to SVG FlameGraph
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > mathstudio_flamegraph.svg
```

---

## 4. macOS Profiling Playbook (`Instruments`)

```bash
# Profile CPU time and thread scheduling on Apple Silicon
xcrun xctrace record --template 'Time Profiler' --launch -- ./build/mathstudio
```

---

## 5. Controlled Profiling Scenarios

| Scenario | Input Expression | Target Focus |
| :--- | :--- | :--- |
| **Scenario A: Idle** | *(None / Window Open)* | Base ImGui & SDL swapchain overhead (verify `astEvaluations == 0`) |
| **Scenario B: Harmonic** | `sin(x)` (600 pts) | Single-curve pipeline latency |
| **Scenario C: Heavy Multi-Term** | `sin(x^2) + cos(x^3) * exp(-x^2)` | AST depth and transcendental math throughput |
| **Scenario D: Multi-Function** | 10 active curves | Scaling linearity and plot buffer updates |
| **Scenario E: Variable Animation** | 5 sliders animating at 60 Hz | Variable store updates and UI frame pacing |
