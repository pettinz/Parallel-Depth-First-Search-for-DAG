#include <queue>
#include <thread>
#include <mutex>

#include "dt.hpp"

mutex m;


unsigned long cmppath(const vector<unsigned long> &a, const vector<unsigned long> &b)
{
    for (auto ita = a.begin(), itb = b.begin(); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (*ita == *itb) continue;

        return (*ita)-(*itb);
    }

    return 0;
}

void compute_path(unsigned long i, unsigned long np, vector<vector<unsigned long>> &path, vector<unsigned long> &parent, queue<unsigned long> &Q)
{
    vector<unsigned long> pri, qri;
    bool update;

    pri.emplace_back(i);
    update = (qri.empty() || cmppath(pri, qri) <= 0);

    unique_lock<mutex> l{m};
    if (update)
    {
        parent[i] = np;
        path[i] = pri;
    }

    if (--parent[i] == 0)
        Q.push(i);
}

DT DT::fromDAG(const DAG &dag)
{
    unsigned long size = dag.get_size();
    vector<unsigned long> IA = dag.get_csr().getIA();
    vector<unsigned long> JA = dag.get_csr().getJA();

    vector<unsigned long> parent;

    vector<vector<unsigned long>> path(size);
    queue<unsigned long> Q;
    
    for (int i = 0; i < size; parent.emplace_back(i++));

    while (!Q.empty())
    {
        queue<unsigned long> C;
        vector<thread> thread_pool;

        while (!Q.empty())
        {
            unsigned long np = Q.front();
            Q.pop();
            thread_pool.emplace_back(thread([&, np] {
                vector<thread> thread_pool;

                for (unsigned long int i = IA[np]; i < IA[np+1]; i++)
                    thread_pool.emplace_back(compute_path, i, np, ref(path), ref(parent), ref(Q));
                
                for (vector<thread>::iterator it = thread_pool.begin(); it != thread_pool.end(); (*it++).join());
            }));
        }
        
        for (vector<thread>::iterator it = thread_pool.begin(); it != thread_pool.end(); it++)
            (*it).join();

        swap(Q, C);
    }

    DT dt;

    return dt;
}