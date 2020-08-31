cmake_minimum_required(VERSION 3.5)
project(Parallel-Depth-First-Search-for-DAG)

find_package(spdlog REQUIRED)

add_library(DAG STATIC dag.cpp)

target_link_libraries(DAG spdlog::spdlog)

target_include_directories(DAG
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(DAG PRIVATE cxx_auto_type)
target_compile_options(DAG PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

add_subdirectory(test)