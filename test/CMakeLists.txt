cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG-tests)

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

file(GLOB GRA_FILES "gra/*.gra")
message(STATUS "Graph files copied into " ${PROJECT_BINARY_DIR})
file(COPY ${GRA_FILES} DESTINATION ${PROJECT_BINARY_DIR})