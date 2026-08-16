# External Dependencies & Vendor Directory

This directory contains third-party open-source libraries used by MathStudio. All libraries are bundled directly to allow zero-dependency, single-command CMake builds across Windows, Linux, and macOS.

## Bundled Libraries

### 1. Dear ImGui (Docking Branch)
- **Path**: `external/imgui/`
- **Version**: 1.90.4 WIP (Docking Branch)
- **License**: [MIT License](https://opensource.org/licenses/MIT)
- **Website**: https://github.com/ocornut/imgui
- **Description**: Bloat-free immediate mode graphical user interface toolkit for C++.
- **Modifications**: Custom backend bindings in `imgui_impl_sdl2.cpp` and `imgui_impl_sdlrenderer2.cpp` adapted for high-DPI scaling and custom dark palette rendering. All original MIT copyright headers remain preserved.

### 2. ImPlot
- **Path**: `external/implot/`
- **Version**: 0.16
- **License**: [MIT License](https://opensource.org/licenses/MIT)
- **Website**: https://github.com/epezent/implot
- **Description**: Advanced 2D plotting extension for Dear ImGui supporting Cartesian, polar, signal, and discrete stem plots.

### 3. FontAwesome 6 Free Vector Icons
- **Path**: `src/ui/fonts/` (`fa-solid-900.ttf`, `FontAwesome6SolidData.h`, `IconsFontAwesome6.h`)
- **License**: [SIL Open Font License (OFL) 1.1](https://scripts.sil.org/OFL) (Font) / [MIT License](https://opensource.org/licenses/MIT) (Code Headers)
- **Website**: https://fontawesome.com
- **Description**: Scalable vector icons embedded directly into C++ binary header files for standalone execution without external font file loading.

---

## Licensing Terms & Compliance

All bundled third-party libraries use permissive open-source licenses (MIT & SIL OFL 1.1). Redistribution, code modification, and commercial use are 100% permitted provided copyright notices are retained.
