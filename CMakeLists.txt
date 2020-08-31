cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

set(CMAKE_CXX_STANDARD 11)

find_package(spdlog REQUIRED)

add_subdirectory(libs)

include_directories(include)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE spdlog::spdlog
    DAG
)