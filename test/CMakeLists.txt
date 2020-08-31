cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

include_directories(include)

add_executable(dag_test dag_test.cpp ../src/dag.cpp)
target_link_libraries(dag_test spdlog::spdlog)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dag_test.gra ${CMAKE_CURRENT_BINARY_DIR}/dag_test.gra COPYONLY)