cmake_minimum_required(VERSION 3.10)
project(FunctionPlotter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

add_executable(plotter
	src/main.cpp
	src/renderer.cpp
	src/math/tokenizer.cpp
	src/math/parser.cpp
	src/math/evaluator.cpp
	src/math/solver.cpp
	src/math/functions.cpp
	src/math/constants_registry.cpp
	src/math/expression.cpp
	src/math/numerical.cpp
)

target_include_directories(plotter PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(plotter PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)
