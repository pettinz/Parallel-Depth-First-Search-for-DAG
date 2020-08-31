#define FILE_TEST "dag_test.gra"
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "dag.hpp"

DAG dag;
CSR csr;

TEST_CASE( "CSR computation", "[csr]" ) {
    vector<unsigned long> test_IA = {0,2,4,6,6,7,8,8};
    vector<unsigned long> test_JA = {1,2,3,4,4,5,6,6};

    dag.readFromFile(FILE_TEST);
    csr = dag.getCSR();

    REQUIRE(csr.getIA() == test_IA);
    REQUIRE(csr.getJA() == test_JA);
}