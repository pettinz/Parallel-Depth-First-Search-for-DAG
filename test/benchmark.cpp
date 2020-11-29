#define CATCH_CONFIG_MAIN

#include <string>
#include <queue>
#include <chrono>
#include <filesystem>
namespace fs = std::filesystem;

#include "catch2/catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"

#ifndef GRA_DIR
#define GRA_DIR ""
#endif

#define TEST_PARALLEL_DFS
#define TEST_LABELING
#define TEST_EXECUTION_TIMES

void getTestFiles(const string &dir, queue<string> &files)
{
    queue<string> files_tmp;

    for (auto &p : fs::directory_iterator(dir))
        if (fs::is_regular_file(p) && p.path().extension() == ".gra")
            files_tmp.push(p.path().string());

    files = move(files_tmp);
}

void getTestFilesRecursive(const string &dir, queue<fs::path> &files)
{
    queue<fs::path> files_tmp;

    for (auto &p : fs::recursive_directory_iterator(dir))
        if (fs::is_regular_file(p) && p.path().extension() == ".gra")
            files_tmp.push(p.path().string());

    files = move(files_tmp);
}

#ifdef TEST_EXECUTION_TIMES
TEST_CASE("Execution times", "dag")
{
    queue<fs::path> files;
    getTestFilesRecursive(GRA_DIR, files);

    spdlog::info("Testing execution times on parallel DFS");
    while (!files.empty())
    {
        fs::path file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file.filename().string());

        DAG dag(file);
        vector<unsigned long> outerRank, innerRank;

        auto start = chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        dag.labeling(outerRank, innerRank);
        auto end = chrono::high_resolution_clock::now();

        spdlog::info("\tcompleted in {}s", chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9);
    }
}
#endif
