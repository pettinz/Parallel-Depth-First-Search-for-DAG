file(GLOB GRA_DIR "gra")

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.3)
FetchContent_MakeAvailable(Catch2)

set(SOURCES "${Parallel-Depth-First-Search-for-DAG_SOURCE_DIR}/src/dag.cpp")
set(HEADERS "${Parallel-Depth-First-Search-for-DAG_SOURCE_DIR}/include/dag.hpp")

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE
    spdlog::spdlog
    Catch2::Catch2
    dag
)
target_include_directories(benchmark PRIVATE "${Parallel-Depth-First-Search-for-DAG_SOURCE_DIR}/include")
target_compile_definitions(benchmark PRIVATE GRA_DIR="${GRA_DIR}/")
target_compile_features(benchmark PUBLIC cxx_std_17)

add_executable(testlib testlib.cpp)
target_link_libraries(testlib PRIVATE
    spdlog::spdlog
    Catch2::Catch2
    dag
)
target_include_directories(testlib PRIVATE "${Parallel-Depth-First-Search-for-DAG_SOURCE_DIR}/include")
target_compile_definitions(testlib PRIVATE GRA_DIR="${GRA_DIR}/")
target_compile_features(testlib PUBLIC cxx_std_17)

add_test(NAME testlibtest COMMAND testlib)