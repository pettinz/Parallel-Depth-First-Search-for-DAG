#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <vector>
#include <queue>

typedef unsigned long node;

using namespace std;

void compute_subgraph_size();
unsigned long dt_size;
vector<node> IA, JA;
vector<unsigned long> subgraph_dt_size;
vector<unsigned long> presum;
vector<node> parents;

// CPP

void thread_nodeQ(node i, vector<node> &to_be_marked, queue<node> &C, queue<node> &L)
{
    unsigned long p = parents[i];
    // lock_guard<mutex> lock(mutexes[p]);

    if (--to_be_marked[p] == 0)
    {
        C.push(p);
        L.push(p);
    }
}

void thread_nodeC(node p)
{
    unsigned long sub = 0;
    unsigned long j = 0;
    for (unsigned long i = IA[p]; i < IA[p + 1]; i++)
    {
        sub += subgraph_dt_size[i];
        if (j > 0)
            presum[i] += subgraph_dt_size[i - 1];
        j++;
    }
    subgraph_dt_size[p] += sub;
}

void compute_subgraph_size()
{
    subgraph_dt_size = vector<node>(dt_size, 1);
    presum = vector<node>(dt_size, 0);
    vector<unsigned long> to_be_marked(dt_size);
    queue<node> Q;

    for (int i = 0; i < dt_size; i++)
    {
        if (IA[i + 1] - IA[i] == 0)
            Q.push(i);
        to_be_marked[i] = IA[i + 1] - IA[i];
    }

    while (!Q.empty())
    {
        queue<unsigned long> C, L;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            thread_nodeQ(p, to_be_marked, Q, L);
        }

        while (!L.empty())
        {
            unsigned long p = L.front();
            L.pop();

            thread_nodeC(p);
        }

        swap(Q, C);
    }
}

TEST_CASE("compute_subgraph_size", "dt")
{
    dt_size = 7;
    IA = vector<unsigned long>{0, 2, 4, 6, 6, 7, 8, 8};
    JA = vector<unsigned long>{1, 2, 3, 4, 4, 5, 6, 6};
    parents = vector<node>{0, 0, 0, 1, 1, 2, 4};

    compute_subgraph_size();

    REQUIRE(subgraph_dt_size == vector<node>{7, 4, 2, 1, 2, 1, 1});
    REQUIRE(presum == vector<node>{0, 0, 4, 0, 1, 0, 0});
}