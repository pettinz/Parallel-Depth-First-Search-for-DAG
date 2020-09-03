/**
 * @file pre_post_order.cpp
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

using namespace std;

typedef unsigned long node;

unsigned long size;
vector<node> parents;
vector<unsigned long> IA, JA;
vector<node> presum, sub_size;

void thread_nodeC(node i, node pre, node post, vector<node> &pre_order, vector<node> &post_order, queue<node> &P)
{
    pre_order[i] = pre + presum[i];
    post_order[i] = post + presum[i];

    P.push(i);
}

void thread_nodeP(node p, unsigned long d, vector<node> &pre_order, vector<node> &post_order, queue<node> &P)
{
    node pre = pre_order[p],
         post = post_order[p];

    for (node i = IA[p]; i < IA[p + 1]; i++)
        thread_nodeC(JA[i], pre, post, pre_order, post_order, P);

    pre_order[p] = pre + d;
    post_order[p] = post + sub_size[p] - 1;
}

void BFS3()
{
    vector<node> pre_order(size, 0), post_order(size, 0);
    queue<node> roots;
    unsigned long depth = 0;

    for (node i = 0; i < size; i++)
        if (parents[i] == i)
            roots.push(i);

    while (!roots.empty())
    {
        queue<node> parents;

        while (!roots.empty())
        {
            node p = roots.front();
            roots.pop();

            thread_nodeP(p, depth, pre_order, post_order, parents);
        }

        swap(roots, parents);
        depth++;
    }
}