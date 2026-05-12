cmake_minimum_required(VERSION 3.10)

project(Homework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES src/main.c src/delivery.c src/dijkstra.c src/graph.c src/heap.c)
add_executable(Homework_app ${SOURCES})
