#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <optional>
#include "spdlog/spdlog.h"
#include "threadsafe_queue.hpp"

#include "threadpool.hpp"

#include "dt.hpp"

ThreadPool threadpool(thread::hardware_concurrency());

long long cmppath(const vector<unsigned long> &, const vector<unsigned long> &);

mutex qm;

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

            tasks.emplace_back(threadpool.enqueue([&, p] {
                vector<future<void>> tasks;

                for (unsigned long int i = IA_dag[p]; i < IA_dag[p + 1]; i++)
                    tasks.emplace_back(threadpool.enqueue([&, p](node i) {
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

    for (unsigned long i = 0, pos = 0; i < parents.size();
         IA.emplace_back(pos), i++)
    {
        for (unsigned long j = 0; j < parents.size(); j++)
            if (i == parents[j] && i != j)
            {
                JA.emplace_back(j);
                pos++;
            }
    }
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