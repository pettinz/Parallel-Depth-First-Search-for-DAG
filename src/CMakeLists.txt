set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/dag.hpp")

add_library(dag dag.cpp ${HEADER_LIST})
target_include_directories(dag PUBLIC ../include)
target_link_libraries(dag PRIVATE
    spdlog::spdlog
    Threads::Threads
    ThreadPool::ThreadPool
    Threadsafe::Threadsafe
)
target_compile_features(dag PUBLIC cxx_std_17)