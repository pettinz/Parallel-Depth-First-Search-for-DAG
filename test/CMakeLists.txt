cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG-tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/dag.cpp
)

add_executable(test test.cpp ${SOURCES})
target_link_libraries(test 
    spdlog::spdlog
    ThreadPool::ThreadPool
    ThreadSafe::Queue
)

add_executable(benchmark benchmark.cpp ${SOURCES})
target_link_libraries(benchmark 
    spdlog::spdlog
    ThreadPool::ThreadPool
    ThreadSafe::Queue
)
target_compile_definitions(benchmark PRIVATE GRA_DIR="${GRA_DIR}/")

