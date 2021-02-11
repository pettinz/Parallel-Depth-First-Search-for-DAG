cmake_minimum_required(VERSION 3.5)

project(
  ThreadSafe
  VERSION 1.0
  DESCRIPTION "Thread-safe management"
  LANGUAGES CXX)

find_package(Threads REQUIRED)

add_library(threadsafe INTERFACE)
add_library(Threadsafe::Threadsafe ALIAS threadsafe)
target_include_directories(threadsafe INTERFACE include/)