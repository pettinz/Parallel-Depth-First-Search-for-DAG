cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

find_package(Catch2 REQUIRED)

add_executable(dag_test dag_test.cpp ../dag.cpp)
target_link_libraries(dag_test Catch2::Catch2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dag_test.gra ${CMAKE_CURRENT_BINARY_DIR}/dag_test.gra COPYONLY)