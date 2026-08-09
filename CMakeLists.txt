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


# --- GUI plotter (requires SDL2 + SDL2_ttf) ---
option(BUILD_GUI "Build the GUI plotter (requires SDL2)" ON)

if(BUILD_GUI)
    # find packages via Config (vcpkg) or Module (System)
    find_package(SDL2 CONFIG QUIET)
    if(NOT SDL2_FOUND)
        find_package(SDL2 QUIET)
    endif()

    find_package(SDL2_ttf CONFIG QUIET)
    if(NOT SDL2_ttf_FOUND)
        find_package(SDL2_ttf QUIET)
    endif()

    # Fallback for Linux/macOS using PkgConfig
    if(NOT SDL2_FOUND AND NOT TARGET SDL2::SDL2)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2 QUIET sdl2)
            pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
        endif()
    endif()

    if(SDL2_FOUND OR TARGET SDL2::SDL2 OR SDL2_INCLUDE_DIRS)

        add_executable(mathstudio
            src/main.cpp
            src/renderer.cpp
            ${MATH_SOURCES}
        )


        # Linking SDL2
        if(TARGET SDL2::SDL2)
            target_link_libraries(mathstudio PRIVATE SDL2::SDL2main SDL2::SDL2)
        elseif(SDL2_LIBRARIES)
            target_link_libraries(mathstudio PRIVATE ${SDL2_LIBRARIES})
            target_include_directories(mathstudio PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()

        # Linking SDL2_ttf
        if(TARGET SDL2_ttf::SDL2_ttf)
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
        endif()

        if(MSVC)
            target_compile_options(mathstudio PRIVATE /W4)
        else()
            target_compile_options(mathstudio PRIVATE -Wall -Wextra -Wpedantic)
        endif()


    else()
        message(FATAL_ERROR "SDL2 and SDL2_ttf development libraries are required to build MathStudio GUI")
    endif()
endif()



# --- Unit tests ---
add_executable(tests tests/test_math.cpp ${MATH_SOURCES})
add_executable(test_ast tests/test_ast.cpp ${MATH_SOURCES})

if(MSVC)
    target_compile_options(tests PRIVATE /W4)
    target_compile_options(test_ast PRIVATE /W4)
else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_ast PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME MathTests COMMAND tests)
add_test(NAME ASTTests COMMAND test_ast)


# --- Benchmarks ---
add_executable(benchmarks benchmarks/bench_v1.cpp ${MATH_SOURCES})
add_executable(bench_v0.2.0 benchmarks/bench_v0.2.0.cpp ${MATH_SOURCES})

if(MSVC)
    target_compile_options(benchmarks PRIVATE /W4)
    target_compile_options(bench_v0.2.0 PRIVATE /W4)
else()
    target_compile_options(benchmarks PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(bench_v0.2.0 PRIVATE -Wall -Wextra -Wpedantic)
endif()


