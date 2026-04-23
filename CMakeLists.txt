cmake_minimum_required(VERSION 3.10)
project(FunctionPlotter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Math sources (shared between plotter and tests) ---
set(MATH_SOURCES
    src/math/tokenizer.cpp
    src/math/parser.cpp
    src/math/evaluator.cpp
    src/math/solver.cpp
    src/math/functions.cpp
    src/math/constants_registry.cpp
    src/math/expression.cpp
    src/math/numerical.cpp
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
        add_executable(plotter
            src/main.cpp
            src/renderer.cpp
            ${MATH_SOURCES}
        )

        # Linking SDL2
        if(TARGET SDL2::SDL2)
            target_link_libraries(plotter PRIVATE SDL2::SDL2main SDL2::SDL2)
        elseif(SDL2_LIBRARIES)
            target_link_libraries(plotter PRIVATE ${SDL2_LIBRARIES})
            target_include_directories(plotter PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()

        # Linking SDL2_ttf
        if(TARGET SDL2_ttf::SDL2_ttf)
            target_link_libraries(plotter PRIVATE SDL2_ttf::SDL2_ttf)
        elseif(SDL2_TTF_LIBRARIES)
            target_link_libraries(plotter PRIVATE ${SDL2_TTF_LIBRARIES})
            target_include_directories(plotter PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
        endif()

        # Extra include path to support <SDL2/SDL.h> format if needed
        # Some vcpkg/system installs only add .../include/SDL2 to the path
        if(SDL2_INCLUDE_DIRS)
            foreach(inc ${SDL2_INCLUDE_DIRS})
                get_filename_component(inc_parent "${inc}" DIRECTORY)
                target_include_directories(plotter PRIVATE "${inc_parent}")
            endforeach()
        endif()

        if(MSVC)
            target_compile_options(plotter PRIVATE /W4)
        else()
            target_compile_options(plotter PRIVATE -Wall -Wextra -Wpedantic)
        endif()

        message(STATUS "SDL2 found -- building plotter GUI")
    else()
        message(STATUS "SDL2 not found -- skipping plotter GUI (tests will still build)")
    endif()
endif()

# --- Unit tests (no SDL2 dependency -- always builds) ---
add_executable(tests tests/test_math.cpp ${MATH_SOURCES})

if(MSVC)
    target_compile_options(tests PRIVATE /W4)
else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME MathTests COMMAND tests)
