#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"
#include <chrono>
#include <filesystem>
#include "amaze.gra.hpp"

using namespace std;

DAG dag(TEST_FILE);
DAG dag2(TEST_FILE);

#ifdef TEST_READ
TEST_CASE("Read", "[dag]")
{
    REQUIRE(dag.getV() == test_V);
    REQUIRE(dag.getIA() == test_IA);
    REQUIRE(dag.getJA() == test_JA);
    REQUIRE(dag.get_np() == test_np);
}
#endif // TEST_READ

TEST_CASE("DFS", "[dag]")
{
    //DAG::DT dt = dag.toDT();

    //const vector<unsigned long> &IA = dt.getIA(), &JA = dt.getJA(), &parents = dt.getParents();

#ifdef SECTION_1
    SECTION("Directed tree generation")
    {
        REQUIRE(parents == test_dtParents);
        REQUIRE(IA == test_dtIA);
        REQUIRE(JA == test_dtJA);
    }
#endif // SECTION_1

    vector<unsigned long> subgraphSize, presum;
    //dt.computeNodeSizeAndPresum(subgraphSize, presum);

#ifdef SECTION_2
    SECTION("Subgraph size computation")
    {
        REQUIRE(subgraphSize == test_subgraph);
        REQUIRE(presum == test_presum);
    }
#endif // SECTION_2

    vector<unsigned long> preorder, postorder, inner, postorder2, inner2;
    //dt.parallelDFS(preorder, postorder);

#ifdef SECTION_3
    SECTION("Pre- and post-order")
    {
        REQUIRE(preorder == test_preorder);
        REQUIRE(postorder == test_postorder);
    }
#endif // SECTION_3
    auto startseq = chrono::high_resolution_clock::now();
    dag2.seqlabeling(postorder2, inner2);
    auto endseq = chrono::high_resolution_clock::now();
    auto start = chrono::high_resolution_clock::now();
    dag.labeling(postorder, inner);
    auto end = chrono::high_resolution_clock::now(); 

    spdlog::info("\tSEQUENTIAL completed in {}s", chrono::duration_cast<chrono::milliseconds>(endseq - startseq).count());

    spdlog::info("\tPARALLEL completed in {}s", chrono::duration_cast<chrono::milliseconds>(end - start).count());

    SECTION("Parallel vs Recursive")
    {
        vector<unsigned long> preorder_r, postorder_r, inner_r, outer_r;
        //dag.DFS(preorder_r, postorder_r, inner_r, outer_r);

        //REQUIRE(preorder == preorder_r);
        REQUIRE(postorder == postorder2);
        REQUIRE(inner == inner2);
    }

}