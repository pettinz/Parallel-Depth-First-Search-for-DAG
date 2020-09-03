cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

include_directories(include)

set(DAG_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dag_test.cpp
    ../src/dag.cpp
)

set(DT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dt_test.cpp
    ../src/dag.cpp
    ../src/dt.cpp
)

add_executable(dag_test ${DAG_TEST_SOURCES})
target_link_libraries(dag_test spdlog::spdlog)

add_executable(dt_test ${DT_TEST_SOURCES})
target_link_libraries(dt_test
    spdlog::spdlog
    ThreadPool::ThreadPool
    ThreadSafe::Queue
)

add_executable(subgraph_test ${CMAKE_CURRENT_SOURCE_DIR}/subgraph_size_test.cpp)
target_link_libraries(dt_test
    spdlog::spdlog
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dag_test.gra ${CMAKE_CURRENT_BINARY_DIR}/dag_test.gra COPYONLY)