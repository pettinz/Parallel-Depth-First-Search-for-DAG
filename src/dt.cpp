#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <optional>
#include "spdlog/spdlog.h"
#include "threadsafe_queue.hpp"

#include "threadpool.hpp"

#include "dt.hpp"

long long cmppath(const vector<unsigned long> &, const vector<unsigned long> &);
mutex m, n;
ThreadPool *threadpool = ThreadPool::getInstance();

DT::DT(const DAG &dag) : size(dag.get_size()), parents(size), mutexes(size)
{
    const vector<unsigned long> &IA_dag = dag.getIA();
    const vector<unsigned long> &JA_dag = dag.getJA();

    vector<unsigned long> parentsCount = dag.get_np();

    vector<vector<node>> path(size);
    queue<node> Q;

    for (unsigned long i = 0; i < size; i++)
    {
        if (parentsCount[i] == 0)
            Q.push(i);

        parents[i] = i;
    }

    while (!Q.empty())
    {
        threadsafe::queue<unsigned long> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpool->enqueue([&, p] {
                vector<future<void>> tasks;

                for (unsigned long int i = IA_dag[p]; i < IA_dag[p + 1]; i++)
                    tasks.emplace_back(threadpool->enqueue([&, p](node i) {
                        lock_guard<mutex> lock(mutexes[i]);

                        vector<node> pri = path[p];
                        vector<node> &qri = path[i];

                        pri.emplace_back(i);

                        if (cmppath(pri, qri) <= 0)
                        {
                            parents[i] = p;
                            qri = pri;
                        }

                        if (--parentsCount[i] == 0)
                            P.push(i);
                    },
                                                           JA_dag[i]));

                for (auto &t : tasks)
                    t.wait();
            }));
        }

        for (auto &t : tasks)
            t.wait();

        P.swap(Q);
    }

    unsigned long pos = 0;
    for (unsigned long i = 0; i < parents.size(); i++)
    {
        IA.emplace_back(pos);

        for (unsigned long j = 0; j < parents.size(); j++)
            if (i == parents[j] && i != j)
            {
                JA.emplace_back(j);
                pos++;
            }
    }
    IA.emplace_back(pos);
}

void DT::computeNodeSize(vector<node> &nodeSize, vector<node> &presum)
{
    vector<node> nodeSize_tmp(size, 1);
    vector<node> presume_tmp(size, 0);
    vector<node> marked(size);
    queue<node> Q;

    for (unsigned long i = 0; i < size; i++)
    {
        if (IA[i + 1] - IA[i] == 0)
            Q.push(i);

        marked[i] = IA[i + 1] - IA[i];
    }

    while (!Q.empty())
    {
        threadsafe::queue<node> C;
        vector<future<void>> tasks[2];

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks[0].emplace_back(threadpool->enqueue([&](node p) {
                lock_guard<mutex> lock(mutexes[p]);
                if (--marked[p] == 0)
                    C.push(p);
            },
                                                      parents[p]));
        }

        for (auto &t : tasks[0])
            t.wait();

        while (!C.empty())
        {
            node p = C.pop().value();
            Q.push(p);

            tasks[1].emplace_back(threadpool->enqueue([&, p] {
                unsigned long sub = 0,
                              j = 0;
                for (unsigned long i = IA[p]; i < IA[p + 1]; i++)
                {
                    sub += nodeSize_tmp[JA[i]];
                    if (j > 0)
                        presume_tmp[JA[i]] += nodeSize_tmp[JA[i - 1]];
                    j++;
                }

                nodeSize_tmp[p] += sub;
            }));
        }

        for (auto &t : tasks[1])
            t.wait();
    }

    nodeSize = move(nodeSize_tmp);
    presum = move(presume_tmp);
}

void DT::computeOrder(vector<unsigned long> &preOrder, vector<unsigned long> &postOrdedr, const vector<node> &nodeSize, const vector<node> &presum)
{
    vector<unsigned long> preOrder_tmp(size, 0), postOrdedr_tmp(size, 0);
    queue<node> Q;
    unsigned long depth = 0;

    for (node i = 0; i < size; i++)
        if (parents[i] == i)
            Q.push(i);

    while (!Q.empty())
    {
        threadsafe::queue<node> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            node p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpool->enqueue([&, p] {
                unsigned long pre = preOrder_tmp[p],
                              post = postOrdedr_tmp[p];

                vector<future<void>> tasks;

                for (node i = IA[p]; i < IA[p + 1]; i++)
                    tasks.emplace_back(threadpool->enqueue([&, pre, post](node i) {
                        preOrder_tmp[i] = pre + presum[i];
                        postOrdedr_tmp[i] = post + presum[i];

                        P.push(i);
                    },
                                                           JA[i]));

                for (auto &t : tasks)
                    t.wait();

                preOrder_tmp[p] = pre + depth;
                postOrdedr_tmp[p] = post + nodeSize[p] - 1;
            }));
        }

        for (auto &t : tasks)
            t.wait();

        P.swap(Q);
        depth++;
    }

    preOrder = move(preOrder_tmp);
    postOrdedr = move(postOrdedr_tmp);
}

long long cmppath(const vector<unsigned long> &a, const vector<unsigned long> &b)
{
    for (auto ita = a.begin(), itb = b.begin(); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (*ita > *itb)
            return 1;

        if (*ita < *itb)
            return -1;
    }

    return 0;
}