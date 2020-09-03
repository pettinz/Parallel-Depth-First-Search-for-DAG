cmake_minimum_required(VERSION 3.5)
project(ThreadSafe LANGUAGES CXX)

find_package( Threads REQUIRED )

add_library(Queue INTERFACE)
add_library(ThreadSafe::Queue ALIAS Queue)

target_link_libraries(Queue INTERFACE Threads::Threads)

target_include_directories(Queue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
