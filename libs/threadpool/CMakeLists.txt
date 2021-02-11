cmake_minimum_required(VERSION 3.4)

project(
    ThreadPool
    VERSION 1.0
    DESCRIPTION "C++ Threadpool"
    LANGUAGES CXX
)

add_library(ThreadPool INTERFACE)
add_library(ThreadPool::ThreadPool ALIAS ThreadPool)
target_include_directories(ThreadPool INTERFACE include/)