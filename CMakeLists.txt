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

add_subdirectory(libs)

add_subdirectory(app)

add_subdirectory(src)

find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

include(CTest)
add_subdirectory(test)