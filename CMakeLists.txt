cmake_minimum_required(VERSION 3.15)
project(Fractal VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- SDL ---
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# --- SOURCES ---
set(FRACTAL_SOURCES
    fractals/factory.cpp
    fractals/mandelbrot.cpp
    fractals/julia.cpp
    fractals/plasma.cpp
    fractals/koch_snowflake.cpp
    fractals/menger.cpp
    fractals/pythagoras.cpp
    fractals/sierpinski.cpp
    fractals/hilbert_curve.cpp
    fractals/animated_tree.cpp
)

add_executable(Fractal
    main.cpp
    ${FRACTAL_SOURCES}
)

# --- INCLUDE PATHS ---
target_include_directories(Fractal PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/fractals
)

# --- LINK ---
target_link_libraries(Fractal
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        m
)

# --- OPTIMIZATIONS ---
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(Fractal PRIVATE -O3 -march=native)
endif()

install(TARGETS Fractal DESTINATION bin)
