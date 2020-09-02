#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include "spdlog/spdlog.h"

#include "dt.hpp"

long long cmppath(const vector<unsigned long> &, const vector<unsigned long> &);
void compute_path(node, node, vector<vector<node>> &, vector<node> &, vector<unsigned long> &, queue<node> &, mutex &m);
void fp();

DT::DT(const DAG &dag)
{
    size = dag.get_size();
    parent.resize(size);
    mutexes = vector<mutex>(size);

    const vector<unsigned long> &IA = dag.get_csr().getIA();
    const vector<unsigned long> &JA = dag.get_csr().getJA();

    vector<unsigned long> parentCount = dag.get_np();

    vector<vector<node>> path(size);
    queue<node> Q;

    for (unsigned long i = 0; i < size; i++)
    {
        if (parentCount[i] == 0)
            Q.push(i);

        parent[i] = i;
    }

    while (!Q.empty())
    {
        queue<node> C;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            vector<thread> tasks[2];

            tasks[0].emplace_back(thread([&, p] {
                for (unsigned long int i = IA[p]; i < IA[p + 1]; i++)
                    tasks[1].emplace_back(thread([&, p](node i) {
                        unique_lock<mutex> l{mutexes[i]};

                        vector<node> pri = path[p];
                        vector<node> &qri = path[i];
                        bool update;

                        pri.emplace_back(i);

                        update = (qri.empty() || cmppath(pri, qri) <= 0);

                        if (update)
                        {
                            parent[i] = p;
                            path[i] = pri;
                        }

                        if (--parentCount[i] == 0)
                            C.push(i);
                    },
                                                 JA[i]));

                for (auto &t : tasks[1])
                    t.join();
            }));

            for (auto &t : tasks[0])
                t.join();
        }

        swap(Q, C);
    }
}

long long cmppath(const vector<unsigned long> &a, const vector<unsigned long> &b)
{
    for (auto ita = a.begin(), itb = b.begin(); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (*ita == *itb)
            continue;

        return (*ita) - (*itb);
    }

    return 0;
}

void compute_path(node i, node p, vector<vector<node>> &path, vector<node> &parent, vector<unsigned long> &parentCount, queue<node> &P, mutex &m)
{
}
