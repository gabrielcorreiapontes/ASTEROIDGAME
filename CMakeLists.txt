cmake_minimum_required(VERSION 3.29)
project(AsteroidsGamePIF C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(AsteroidsGamePIF ${SOURCES})

target_link_libraries(AsteroidsGamePIF m)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
