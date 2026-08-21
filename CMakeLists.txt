cmake_minimum_required(VERSION 3.10)
project(MathStudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# --- Math sources (shared between mathstudio, tests, and benchmarks) ---
set(MATH_SOURCES
    src/math/tokenizer.cpp
    src/math/parser.cpp
    src/math/evaluator.cpp
    src/math/solver.cpp
    src/math/functions.cpp
    src/math/constants_registry.cpp
    src/math/expression.cpp
    src/math/numerical.cpp
    src/core/MathValue.cpp
    src/core/VariableStore.cpp
    src/core/FunctionRegistry.cpp
    src/core/ExpressionCache.cpp
    src/core/performance/FrameProfiler.cpp
    src/compiler/ast/Node.cpp
    src/compiler/diagnostics/Diagnostics.cpp
    src/compiler/parser/PrattParser.cpp
    src/compiler/visitors/EvalVisitor.cpp
    src/compiler/visitors/PrintVisitor.cpp
    src/compiler/passes/PassManager.cpp
    src/compiler/passes/ConstantFoldVisitor.cpp
    src/compiler/passes/SimplifyVisitor.cpp
    legacy/shunting_yard.cpp
)


set(UI_SOURCES
    src/ui/UIManager.cpp
    src/ui/ThemeManager.cpp
    src/ui/WorkspaceManager.cpp
    src/ui/CommandPalette.cpp
    src/ui/widgets/ExpressionEditorWidget.cpp
    src/ui/widgets/PlotToolbarWidget.cpp
    src/ui/widgets/AxisEditorWidget.cpp
    src/ui/widgets/VariableTableWidget.cpp
    src/ui/widgets/ConsoleWidget.cpp
    src/ui/widgets/ProfilerWidget.cpp
    src/ui/widgets/FPSGraphWidget.cpp
    src/ui/widgets/StatusBarWidget.cpp
    src/ui/panels/CanvasPanel.cpp
    src/ui/panels/ControlPanel.cpp
    src/ui/panels/VariableInspectorPanel.cpp
    src/ui/panels/PerformanceProfilerPanel.cpp
    src/ui/panels/DiagnosticsConsolePanel.cpp
    src/ui/panels/PlotPanel.cpp
)

# --- Dear ImGui (docking branch) & ImPlot ---
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/implot)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
)

# --- GUI plotter (requires SDL2 + SDL2_ttf) ---
option(BUILD_GUI "Build the GUI plotter (requires SDL2)" ON)

if(BUILD_GUI)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-ttf CONFIG QUIET)
    if(NOT sdl2-ttf_FOUND AND NOT TARGET SDL2_ttf::SDL2_ttf AND NOT TARGET SDL2_ttf::SDL2_ttf-static)
        find_package(SDL2_ttf QUIET)
    endif()

    if(TRUE)

        add_executable(mathstudio
            src/main.cpp
            src/cli/CliParser.cpp
            src/cli/CliRunner.cpp
            src/app/Application.cpp
            src/renderer.cpp
            ${MATH_SOURCES}
            ${UI_SOURCES}
            ${IMGUI_SOURCES}
        )
        target_include_directories(mathstudio PRIVATE
            src
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
            ${IMPLOT_DIR}
        )


        # Linking SDL2
        if(TARGET SDL2::SDL2-static)
            target_link_libraries(mathstudio PRIVATE SDL2::SDL2main SDL2::SDL2-static)
        elseif(TARGET SDL2::SDL2)
            target_link_libraries(mathstudio PRIVATE SDL2::SDL2main SDL2::SDL2)
        elseif(SDL2_LIBRARIES)
            target_link_libraries(mathstudio PRIVATE ${SDL2_LIBRARIES})
            target_include_directories(mathstudio PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()

        # Linking SDL2_ttf
        if(TARGET SDL2_ttf::SDL2_ttf-static)
            target_link_libraries(mathstudio PRIVATE SDL2_ttf::SDL2_ttf-static)
        elseif(TARGET SDL2_ttf::SDL2_ttf)
            target_link_libraries(mathstudio PRIVATE SDL2_ttf::SDL2_ttf)
        elseif(SDL2_TTF_LIBRARIES)
            target_link_libraries(mathstudio PRIVATE ${SDL2_TTF_LIBRARIES})
            target_include_directories(mathstudio PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
        elseif(SDL2_ttf_INCLUDE_DIRS)
            target_include_directories(mathstudio PRIVATE ${SDL2_ttf_INCLUDE_DIRS})
        endif()

        # Add SDL2 subdirectories to include path to support <SDL.h> style
        if(SDL2_INCLUDE_DIRS)
            target_include_directories(mathstudio PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()
        if(SDL2_TTF_INCLUDE_DIRS)
            target_include_directories(mathstudio PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
        endif()
        if(SDL2_ttf_INCLUDE_DIRS)
            target_include_directories(mathstudio PRIVATE ${SDL2_ttf_INCLUDE_DIRS})
        endif()

        if(WIN32)
            target_link_libraries(mathstudio PRIVATE dwmapi)
        elseif(APPLE)
            target_link_directories(mathstudio PRIVATE /opt/homebrew/lib /usr/local/lib)
        endif()

        if(MSVC)
            target_compile_options(mathstudio PRIVATE /W4 /WX /wd4996 /wd4100 /wd4127 /wd4201)
        else()
            target_compile_options(mathstudio PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
        endif()


    else()
        message(FATAL_ERROR "SDL2 and SDL2_ttf development libraries are required to build MathStudio GUI")
    endif()
endif()



# --- Unit tests ---
add_executable(tests tests/test_math.cpp ${MATH_SOURCES})
add_executable(test_ast tests/test_ast.cpp ${MATH_SOURCES})
target_include_directories(tests PRIVATE src)
target_include_directories(test_ast PRIVATE src)

if(MSVC)
    target_compile_options(tests PRIVATE /W4 /WX /wd4996 /wd4100)
    target_compile_options(test_ast PRIVATE /W4 /WX /wd4996 /wd4100)
else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
    target_compile_options(test_ast PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
endif()

enable_testing()
add_test(NAME MathTests COMMAND tests)
add_test(NAME ASTTests COMMAND test_ast)


# --- Benchmarks ---
add_executable(benchmarks benchmarks/bench_v1.cpp ${MATH_SOURCES})
add_executable(bench_v0.2.0 benchmarks/bench_v0.2.0.cpp ${MATH_SOURCES})
add_executable(bench_v0.3.0 benchmarks/bench_v0.3.0.cpp ${MATH_SOURCES})
target_include_directories(benchmarks PRIVATE src)
target_include_directories(bench_v0.2.0 PRIVATE src)
target_include_directories(bench_v0.3.0 PRIVATE src)

if(MSVC)
    target_compile_options(benchmarks PRIVATE /W4 /WX /wd4996 /wd4100)
    target_compile_options(bench_v0.2.0 PRIVATE /W4 /WX /wd4996 /wd4100)
    target_compile_options(bench_v0.3.0 PRIVATE /W4 /WX /wd4996 /wd4100 /wd4244)
else()
    target_compile_options(benchmarks PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
    target_compile_options(bench_v0.2.0 PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
    target_compile_options(bench_v0.3.0 PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
endif()


