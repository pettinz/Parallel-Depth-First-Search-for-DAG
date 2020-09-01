#define FILE_TEST "dag_test.gra"
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "dag.hpp"

CSR csr;

TEST_CASE( "DAG from file", "[dag]" ) {

    DAG dag = DAG::fromFile(FILE_TEST);

    SECTION( "CSR computation" ) {
        vector<unsigned long> test_IA = {0,2,4,6,6,7,8,8};
        vector<unsigned long> test_JA = {1,2,3,4,4,5,6,6};

        csr = dag.get_csr();

        REQUIRE(csr.getIA() == test_IA);
        REQUIRE(csr.getJA() == test_JA);
    }

    SECTION( "get_size" ) {
        REQUIRE(dag.get_size() == 7);
    }

    SECTION ( "Store the number of parents for each node" ) {
        vector<unsigned long> test_np = {0,1,1,1,2,1,2};

        REQUIRE(test_np == dag.get_np());
    }
}