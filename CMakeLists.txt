cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs)

include_directories(include)

find_package( Threads REQUIRED )

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dag.cpp
)

add_executable(main src/main.cpp ${SOURCES})
target_link_libraries(main PRIVATE
    spdlog::spdlog
    Threads::Threads
    ThreadPool::ThreadPool
    ThreadSafe::Queue)

add_subdirectory(test)