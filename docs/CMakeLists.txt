set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md")

doxygen_add_docs(docs
                 "${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md"
                 "include/dag.hpp"
                 "libs/threadpool/include/threadpool.hpp"
                 "libs/threadsafe/include/threadsafe/queue.hpp"
                 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/")