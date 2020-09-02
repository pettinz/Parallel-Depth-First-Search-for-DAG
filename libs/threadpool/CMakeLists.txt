
cmake_minimum_required(VERSION 3.5)
project(ThreadPool LANGUAGES CXX)

find_package( Threads REQUIRED )

add_library(ThreadPool INTERFACE)
add_library(ThreadPool::ThreadPool ALIAS ThreadPool)

target_link_libraries(ThreadPool INTERFACE Threads::Threads)

target_include_directories(ThreadPool INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
