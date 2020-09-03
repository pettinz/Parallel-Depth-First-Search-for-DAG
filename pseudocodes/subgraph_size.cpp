/**
 * @file subgraph_size.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-03
 * 
 * @copyright Copyright (c) 2020
 * 
 * The algorithm is sequential.
 * Be careful to test it before translating it into a sequential one.
 */

#include <vector>
#include <queue>

typedef unsigned long node;

using namespace std;

void compute_subgraph_size();
unsigned long size;
vector<node> IA, JA;
vector<unsigned long> subgraph_size;
vector<unsigned long> presum;
vector<node> parents;

// CPP

void thread_nodeQ(node p, vector<node> &to_be_marked, queue<node> &C)
{
    unsigned long p = parents[p];
    // lock_guard<mutex> lock(mutexes[p]);

    if (--to_be_marked[p] == 0)
        C.push(p);
}

void thread_nodeC(node p)
{
    unsigned long sub = 0;
    unsigned long j = 0;
    for (unsigned long i = IA[p]; i < IA[p + 1]; i++)
    {
        sub += subgraph_size[JA[i]];
        if (j > 0)
            presum[JA[i]] += subgraph_size[JA[i-1]];
        j++;
    }
    subgraph_size[p] += sub;
}

void compute_subgraph_size()
{
    subgraph_size = vector<node>(size, 1);
    presum = vector<node>(size, 0);
    vector<unsigned long> to_be_marked = vector<unsigned long>(size);
    queue<node> Q;

    for (int i = 0; i < size; i++)
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

            thread_nodeQ(p, to_be_marked, Q);
        }

        while (!C.empty())
        {
            unsigned long p = C.front();
            C.pop();

            thread_nodeC(p);
        }

        swap(Q, C);
    }
}