cmake_minimum_required(VERSION 3.10)

project(learning)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(TARGET_NAME app)

find_package(SDL2 REQUIRED)

add_executable(${TARGET_NAME}
    src/main.c
    src/bounce.c
    src/rects.c
    src/expression.c
    src/config.c
)

target_compile_options(${TARGET_NAME} PRIVATE
    -Wall
    -Werror
    -O2
)

target_include_directories(${TARGET_NAME} PRIVATE src)

target_link_libraries(${TARGET_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${TARGET_NAME} PRIVATE m)