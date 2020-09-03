#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"
#include "dt.hpp"

using namespace std;

/**
mutex m;

long long cmppath(const vector<node> &a, const vector<node> &b)
{
    for (auto ita = a.begin(), itb = b.begin(); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (*ita == *itb) continue;

        return (*ita)-(*itb);
    }

    return 0;
}

void compute_path(node i, node p, vector<vector<node>> &path, vector<unsigned long> &parent, vector<unsigned long> &parent_count, queue<node> &P)
{
    vector<node> pri = path[p];
    vector<node> &qri = path[i];
    bool update;

    pri.emplace_back(i);
    update = (qri.empty() || cmppath(pri, qri) <= 0);
    cout << "qri: " << qri.empty() << endl;
    cout << "cmpath: " << cmppath(pri,qri) << endl;

    unique_lock<mutex> l{m};
    if (update)
    {
        parent[i] = p;
        qri = pri;
    }

    if (--parent_count[i] == 0)
        P.push(i);
}

TEST_CASE( "cmppath", "[dag]" ) {
    vector<node> pri = {0,1,4};
    vector<node> qri = {0,2,4};

    REQUIRE(cmppath(pri, qri) == -1);
}

TEST_CASE( "compute_path", "[dag]" ) {
    SECTION ("Without compare") {
        size_t size = 7;
        queue<node> P, required_queue;
        node p = 0, i = 1;
        vector<node> parent = {0,1,2,3,4,5,6};
        vector<unsigned long> parent_count = {0,1,1,1,2,1,3};
        vector<vector<node>> path(size);
        path[0] = {0};
        compute_path(i, p, path, parent, parent_count, P);

        required_queue.push(1);
        REQUIRE(P == required_queue);
        REQUIRE(parent_count == vector<node>{0,0,1,1,2,1,3});
        REQUIRE(parent == vector<node>{0,0,2,3,4,5,6});
        REQUIRE(path[i] == vector<node>{0,1});
    }

    SECTION ("With compare") {
        size_t size = 7;
        queue<node> P, required_queue;
        P.push(3);
        P.push(5);
        required_queue = P;
        node p = 1, i = 4;
        vector<node> parent = {0,0,0,1,2,2,6};
        vector<unsigned long> parent_count = {0,0,0,0,1,0,2};
        vector<vector<node>> path(size);
        path[1] = {0,1};
        path[2] = {0,2};
        path[4] = {0,2,4};
        compute_path(i, p, path, parent, parent_count, P);

        required_queue.push(4);
        REQUIRE(P == required_queue);
        REQUIRE(parent_count == vector<node>{0,0,0,0,0,0,2});
        REQUIRE(parent == vector<node>{0,0,0,1,1,2,6});
        REQUIRE(path[4] == vector<node>{0,1,4});
    }
}
*/

const string DAG_TEST_FILE = "dag_test.gra";
DAG dag(DAG_TEST_FILE);

TEST_CASE("DT::fromDAG", "dt")
{
    for (int i = 0; i < 1000; i++)
    {
        DT dt(dag);
        REQUIRE(dt.getIA() == vector<unsigned long>{0, 2, 4, 5, 5, 6, 6, 6});
        REQUIRE(dt.getJA() == vector<unsigned long>{1, 2, 3, 4, 5, 6});
        REQUIRE(dt.getParent() == vector<node>{0, 0, 0, 1, 1, 2, 4});
    }
}

TEST_CASE("DT::computeNodeSize", "dt")
{
    DT dt(dag);

    vector<node> nodeSize;
    vector<node> presum;

    for (int i = 0; i < 1000; i++)
    {
        dt.computeNodeSize(nodeSize, presum);

        REQUIRE(nodeSize == vector<node>{7, 4, 2, 1, 2, 1, 1});
        REQUIRE(presum == vector<node>{0, 0, 4, 0, 1, 0, 0});
    }
}

TEST_CASE("DT::computeOrder", "dt")
{
    DT dt(dag);

    vector<node> nodeSize = {7, 4, 2, 1, 2, 1, 1};
    vector<node> presum = {0, 0, 4, 0, 1, 0, 0};
    vector<unsigned long> preOrder, postOrder;

    for (int i = 0; i < 1000; i++)
    {
        dt.computeOrder(preOrder, postOrder, nodeSize, presum);

        REQUIRE(preOrder == vector<node>{0, 1, 5, 2, 3, 6, 4});
        REQUIRE(postOrder == vector<node>{6, 3, 5, 0, 2, 4, 1});
    }
}