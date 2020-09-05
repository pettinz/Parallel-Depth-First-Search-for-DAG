#define CATCH_CONFIG_MAIN
#define private public

#include "catch.hpp"
#include "spdlog/spdlog.h"
#include "dag.hpp"
#include "quer.gra.hpp"

DAG dag(TEST_FILE);

TEST_CASE("Read", "[dag]")
{
    REQUIRE(dag.getV() == test_V);
    REQUIRE(dag.getIA() == test_IA);
    REQUIRE(dag.getJA() == test_JA);
    REQUIRE(dag.get_np() == test_np);
}

TEST_CASE("DFS", "[dag]")
{

    vector<node> IA, JA, parents;
    dag.ParallelDFSUtil1(IA, JA, parents);

#ifdef SECTION_1
    SECTION("Directed tree generation")
    {

        REQUIRE(IA == test_dtIA);
        REQUIRE(JA == test_dtJA);
        REQUIRE(parents == test_dtParents);
    }
#endif // SECTION_1

    vector<unsigned long> subgraphSize, presum;
    dag.ParallelDFSUtil2(IA, JA, parents, subgraphSize, presum);

#ifdef SECTION_2
    SECTION("Subgraph size computation")
    {
        REQUIRE(subgraphSize == test_subgraph);
        REQUIRE(presum == test_presum);
    }
#endif // SECTION_2

    vector<unsigned long> preorder, postorder;
    dag.ParallelDFSUtil3(IA, JA, parents, subgraphSize, presum, preorder, postorder);

#ifdef SECTION_3
    SECTION("Pre- and post-order")
    {
        REQUIRE(preorder == test_preorder);
        REQUIRE(postorder == test_postorder);
    }
#endif // SECTION_3

    SECTION("Parallel vs Recursive")
    {
        vector<unsigned long> preorder_r, postorder_r;
        dag.DFS(preorder_r, postorder_r);

        REQUIRE(preorder == preorder_r);
        REQUIRE(postorder == postorder_r);
    }
}