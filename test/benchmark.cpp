#define CATCH_CONFIG_MAIN

#include <string>
#include <chrono>

#include "catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"

void getTestFiles(queue<string> &files)
{
    queue<string> files_tmp;

    for (auto &p : filesystem::recursive_directory_iterator("."))
        if (p.path().extension() == ".gra")
            files_tmp.push(p.path().filename());

    files = move(files_tmp);
}

TEST_CASE("files", "f")
{
    queue<string> files;
    getTestFiles(files);

    while (!files.empty())
    {
        string file = files.front();
        files.pop();

        spdlog::info("Testing file {}...", file);

        DAG dag(file);

        vector<unsigned long> preorder, preorder_r, postorder, postorder_r, inner_r, outer_r, inner;

        dag.DFS(preorder_r, postorder_r, inner_r, outer_r);

        auto start = chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        dag.ParallelDFS(preorder, postorder);
        dag.labeling(postorder, inner);
        auto end = chrono::high_resolution_clock::now();

        REQUIRE(preorder == preorder_r);
        REQUIRE(postorder == outer_r);
        REQUIRE(inner == inner_r);

        spdlog::info("Completed {} in {}s", file, chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9);
    }
}
