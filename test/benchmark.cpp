#define CATCH_CONFIG_MAIN

#include <string>
#include <queue>
#include <chrono>
#include <filesystem>

#include "catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"

const string GRA_DIR{"gra/"};
const string LARGE_REAL_FILES{"large_real/"};
const string SMALL_DENSE_REAL_FILES{"small_dense_real/"};
const string SMALL_SPARSE_REAL_FILES{"small_sparse_real/"};

const string TEST_FILES = GRA_DIR + LARGE_REAL_FILES;

#define TEST_EXECUTION_TIMES

void getTestFiles(const string &dir, queue<string> &files)
{
    queue<string> files_tmp;

    for (auto &p : filesystem::directory_iterator(dir))
        if (p.path().extension() == ".gra")
            files_tmp.push(p.path().filename());

    files = move(files_tmp);
}

#ifdef TEST_PARALLEL_DFS
TEST_CASE("parallelDFS", "f")
{
    queue<string> files;
    getTestFiles(TEST_FILES, files);

    spdlog::info("Testing parallel DFS");
    while (!files.empty())
    {
        string file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file);

        DAG dag(TEST_FILES + file);

        vector<unsigned long> preorder[2], postorder[2], innerRank, outerRank;

        dag.DFS(preorder[0], postorder[0], innerRank, outerRank);

        dag.parallelDFS(preorder[1], postorder[1]);

        REQUIRE(preorder[0] == preorder[1]);
        REQUIRE(postorder[0] == postorder[1]);

        spdlog::info("\tcompleted.");
    }
}
#endif

#ifdef TEST_LABELING
TEST_CASE("labeling", "dag")
{
    queue<string> files;
    getTestFiles(files);

    spdlog::info("Testing labeling");
    while (!files.empty())
    {
        string file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file);

        DAG dag(file);

        vector<unsigned long> preorder, postorder, outerRank[2], innerRank[2];

        dag.DFS(preorder, postorder, innerRank[1], outerRank[1]);

        auto start = chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        dag.labeling(outerRank[0], innerRank[0]);
        auto end = chrono::high_resolution_clock::now();

        REQUIRE(outerRank[0] == outerRank[1]);
        REQUIRE(innerRank[0] == innerRank[1]);

        spdlog::info("\tcompleted {} in {}s", file, chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9);
    }
}
#endif

#ifdef TEST_EXECUTION_TIMES
TEST_CASE("parallelDFS", "execution time")
{
    queue<string> files;
    getTestFiles(TEST_FILES, files);

    spdlog::info("Testing execution times on parallel DFS");
    while (!files.empty())
    {
        string file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file);

        DAG dag(TEST_FILES + file);
        vector<unsigned long> outerRank, innerRank;

        auto start = chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        dag.labeling(outerRank, innerRank);
        auto end = chrono::high_resolution_clock::now();

        spdlog::info("\tcompleted {} in {}s", file, chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9);
    }
}
#endif
