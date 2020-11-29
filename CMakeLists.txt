cmake_minimum_required(VERSION 3.14...3.18)
project(
    Parallel-Depth-First-Search-for-DAG
    VERSION 1.0
    DESCRIPTION "Project SDP class"
    LANGUAGES CXX
)

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.8.1)
    
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    threadpool
    GIT_REPOSITORY https://github.com/pettinz/threadpool.git
    GIT_TAG v1.0)
FetchContent_MakeAvailable(threadpool)

FetchContent_Declare(
    threadsafe
    GIT_REPOSITORY https://github.com/pettinz/threadsafe.git
    GIT_TAG v1.0)
FetchContent_MakeAvailable(threadsafe)

add_subdirectory(src)

include(CTest)
add_subdirectory(test)