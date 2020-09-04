#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"
#include "threadpool.hpp"
#include "threadsafe_queue.hpp"

#include "spdlog/spdlog.h"

static long long cmppath(const vector<unsigned long> &a, const vector<unsigned long> &b)
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

void DAG::DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder)
{
    visited[v] = true;
    preorder[v] = pre++;

    for (unsigned long i = IA_[v]; i < IA_[v + 1]; i++)
        if (!visited[JA_[i]])
            DFSUtil(JA_[i], pre, post, visited, preorder, postorder);

    postorder[v] = post++;
}

void DAG::DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder)
{
    vector<bool> visited(V_, false);
    vector<unsigned long> preorder_tmp(V_), postorder_tmp(V_);
    unsigned long pre = 0, post = 0;

    for (node i = 0; i < V_; i++)
        if (visited[i] == false && np_[i] == 0)
            DFSUtil(i, pre, post, visited, preorder_tmp, postorder_tmp);

    preorder = move(preorder_tmp);
    postorder = move(postorder_tmp);
}

DAG::DAG(const string &fileName)
{
    ifstream infile(fileName);
    if (!infile.is_open())
    {
        spdlog::critical("Error opening file {}", fileName);
        abort();
    }

    string line;
    unsigned long i, j;
    unsigned long n;

    n = 0;
    IA_.emplace_back(n);

    getline(infile, line);
    V_ = stoi(line);

    np_.resize(V_);

    while (getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> i;
        iss.seekg(sizeof(char), ios_base::cur);

        while (iss >> j)
        {
            JA_.emplace_back(j);

            np_[j]++;
            n++;
        }

        IA_.emplace_back(n);
    }

    // Finding roots
    for (node i = 0; i < V_; i++)
        if (np_[i] == 0)
            roots_.emplace_back(i);

    infile.close();
}

void DAG::ParallelDFS(vector<node> &preorder, vector<node> &postorder)
{
}

void DAG::ParallelDFSUtil1(vector<node> &dtIA, vector<node> &dtJA, vector<node> &dtParents)
{
    vector<node> IA, JA;
    queue<node> Q;
    vector<vector<node>> path(V_);
    vector<mutex> mutexes(V_);
    vector<node> parents(V_);
    vector<node> np = np_;

    ThreadPool *threadpool = ThreadPool::getInstance();

    for (auto n : roots_)
        Q.push(n);

    while (!Q.empty())
    {
        threadsafe::queue<node> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpool->enqueue([&, p] {
                vector<future<void>> tasks;

                for (unsigned long int i = IA_[p]; i < IA_[p + 1]; i++)
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

                        if (--np[i] == 0)
                            P.push(i);
                    },
                                                           JA_[i]));

                for (auto &t : tasks)
                    t.wait();
            }));
        }

        for (auto &t : tasks)
            t.wait();

        P.swap(Q);
    }

    unsigned long pos = 0;
    for (node i = 0; i < V_; i++)
    {
        IA.emplace_back(pos);

        for (node j = 0; j < V_; j++)
            if (i == parents[j] && i != j)
            {
                JA.emplace_back(j);
                pos++;
            }
    }
    IA.emplace_back(pos);

    dtIA = move(IA);
    dtJA = move(JA);
    dtParents = move(parents);
}

void DAG::ParallelDFSUtil2(const vector<node> &dtIA, const vector<node> &dtJA, const vector<node> &dtParents, vector<unsigned long> &nodeSize, vector<unsigned long> &presum)
{
    vector<unsigned long> nodeSize_tmp(V_, 1), presum_tmp(V_, 0), marked(V_);
    vector<mutex> mutexes(V_);
    queue<node> Q;

    ThreadPool *threadpool = ThreadPool::getInstance();

    for (node i = 0; i < V_; i++)
    {
        if ((marked[i] = dtIA[i + 1] - dtIA[i]) == 0)
            Q.push(i);
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
                                                      dtParents[p]));
        }
        for (auto &t : tasks[0])
            t.wait();

        while (!C.empty())
        {
            node p = C.pop().value();
            Q.push(p);

            tasks[1].emplace_back(threadpool->enqueue([&, p] {
                unsigned long sub = 0;
                for (unsigned long i = dtIA[p], j = 0; i < dtIA[p + 1]; i++)
                {
                    sub += nodeSize_tmp[dtJA[i]];
                    if (j++ > 0)
                        presum_tmp[dtJA[i]] = presum_tmp[dtJA[i]] + presum_tmp[dtJA[i - 1]] + nodeSize_tmp[dtJA[i - 1]];
                }

                nodeSize_tmp[p] += sub;
            }));
        }

        for (auto &t : tasks[1])
            t.wait();
    }

    nodeSize = move(nodeSize_tmp);
    presum = move(presum_tmp);
}

void DAG::ParallelDFSUtil3(const vector<node> &dtIA, const vector<node> &dtJA, const vector<node> &dtParents, const vector<unsigned long> &nodeSize, const vector<unsigned long> &presum, vector<unsigned long> &preorder, vector<unsigned long> &postorder)
{
    vector<unsigned long> preorder_tmp(V_, 0), postorder_tmp(V_, 0);
    unsigned long depth = 0;
    queue<node> Q;

    ThreadPool *threadpool = ThreadPool::getInstance();

    for (auto n : roots_)
        Q.push(n);

    while (!Q.empty())
    {
        threadsafe::queue<node> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            node p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpool->enqueue([&, p] {
                unsigned long pre = preorder_tmp[p], post = postorder_tmp[p];
                vector<future<void>> tasks;

                for (unsigned long i = dtIA[p]; i < dtIA[p + 1]; i++)
                    tasks.emplace_back(threadpool->enqueue([&, pre, post](node i) {
                        preorder_tmp[i] = pre + presum[i];
                        postorder_tmp[i] = post + presum[i];

                        P.push(i);
                    },
                                                           dtJA[i]));

                for (auto &t : tasks)
                    t.wait();

                preorder_tmp[p] = pre + depth;
                postorder_tmp[p] = post + nodeSize[p] - 1;
            }));
        }

        for (auto &t : tasks)
            t.wait();

        P.swap(Q);
        depth++;
    }

    preorder = move(preorder_tmp);
    postorder = move(postorder_tmp);
}