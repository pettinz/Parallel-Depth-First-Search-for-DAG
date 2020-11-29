#define CATCH_CONFIG_MAIN

#ifndef GRA_DIR
#define GRA_DIR ""
#endif
#define TEST_PARALLEL_DFS
#define TEST_LABELING

#include <string>
#include <queue>
#include <filesystem>
namespace fs = std::filesystem;

#include "catch2/catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"

using namespace std;

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

#ifdef TEST_PARALLEL_DFS
TEST_CASE("parallelDFS", "f")
{
    queue<fs::path> files;
    getTestFilesRecursive(GRA_DIR, files);

    spdlog::info("Testing parallel DFS");
    while (!files.empty())
    {
        fs::path file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file.filename().string());

        DAG dag(file.string());

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
    queue<fs::path> files;
    getTestFilesRecursive(GRA_DIR, files);

    spdlog::info("Testing labeling");
    while (!files.empty())
    {
        fs::path file = files.front();
        files.pop();

        spdlog::info("...on file {}...", file.filename().string());

        DAG dag(file);

        vector<unsigned long> preorder, postorder, outerRank[2], innerRank[2];

        dag.DFS(preorder, postorder, innerRank[1], outerRank[1]);

        auto start = chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        dag.labeling(outerRank[0], innerRank[0]);
        auto end = chrono::high_resolution_clock::now();

        REQUIRE(outerRank[0] == outerRank[1]);
        REQUIRE(innerRank[0] == innerRank[1]);

        spdlog::info("\tcompleted.");
    }
}
#endif