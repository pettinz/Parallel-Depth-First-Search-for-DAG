#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>
#include <optional>
#include <climits>

#include "dag.hpp"
#include "threadpool.h"
#include "threadsafe/queue.h"

#include "spdlog/spdlog.h"

#define PARAL 4

ThreadPool threadpools[4] = {
    ThreadPool(12),
    ThreadPool(4),
    ThreadPool(6),
    ThreadPool(4)};

bool DAG::swapPath(const vector<unsigned long> &a, const vector<unsigned long> &b)
{
    if (b.empty())
        return true;

    for (auto ita = a.begin(), itb = b.begin(); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (*ita < *itb)
            return true;

        if (*ita > *itb)
            return false;
    }

    return false;
}

void DAG::DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank)
{
    visited[v] = true;
    preorder[v] = pre++;
    unsigned long min = LONG_MAX;

    for (unsigned long i = IA_[v]; i < IA_[v + 1]; i++)
    {
        if (!visited[JA_[i]])
        {
            //spdlog::info("father: {} - child: {}", v, JA_[i]);
            DFSUtil(JA_[i], pre, post, visited, preorder, postorder, innerRank, outerRank);
        }
        if (innerRank[JA_[i]] < min)
            min = innerRank[JA_[i]];
    }

    innerRank[v] = min;
    postorder[v] = post++;
    outerRank[v] = post;
    if (outerRank[v] < min) // foglia
        innerRank[v] = outerRank[v];
}

void DAG::DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank)
{
    vector<bool> visited(V_, false);
    vector<unsigned long> preorder_tmp(V_), postorder_tmp(V_), innerRank_tmp(V_), outerRank_tmp(V_);
    unsigned long pre = 0, post = 0;

    for (unsigned long i = 0; i < V_; i++)
        if (isRoot(i) && visited[i] == false) // && np_[i] == 0)
            DFSUtil(i, pre, post, visited, preorder_tmp, postorder_tmp, innerRank_tmp, outerRank_tmp);

    preorder = move(preorder_tmp);
    postorder = move(postorder_tmp);
    innerRank = move(innerRank_tmp);
    outerRank = move(outerRank_tmp);
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
    unsigned long n, k;
    vector<unsigned long> KA_;

    n = 0;
    k = 0;
    IA_.emplace_back(n);

    getline(infile, line);
    V_ = stoi(line);

    vector<unsigned long> np(V_, 0);

    while (getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> i;
        iss.seekg(sizeof(char), ios_base::cur);

        while (iss >> j)
        {
            JA_.emplace_back(j);
            KA_.emplace_back(k);
            np[j]++;
            n++;
        }
        k++;
        IA_.emplace_back(n);
    }

    // Computing npsum_ as
    npsum_ = vector<unsigned long>(V_ + 1, 0);
    for (unsigned long i = 1; i < V_ + 1; npsum_[i] = npsum_[i - 1] + np[i - 1], i++)
        ;

    // building NA
    NA_ = vector<unsigned long>(n);
    vector<unsigned long> cc(V_, 0);
    unsigned long pos;
    for (unsigned long j = 0, el = JA_[j]; j < n; el = JA_[++j])
    {
        pos = npsum_[el] + cc[el];
        cc[el]++;
        NA_[pos] = KA_[j];
    }

    infile.close();
}

void DAG::parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder) { toDT().parallelDFS(preorder, postorder); }

void DAG::labeling(vector<unsigned long> &outerRank, vector<unsigned long> &innerRank)
{
    vector<unsigned long> preorder, postorder;
    parallelDFS(preorder, postorder);

    vector<unsigned long> innerRank_tmp, marked;
    queue<unsigned long> Q;
    vector<mutex> mutexes(V_);

    for (unsigned long i = 0; i < V_; i++)
    {
        if (IA_[i + 1] == IA_[i])
            Q.push(i);

        marked.emplace_back(IA_[i + 1] - IA_[i]);
        innerRank_tmp.emplace_back(++postorder[i]);
    }

    while (!Q.empty())
    {
        //vector<thread> tasks[2];
        vector<future<void>> tasks[2];
        threadsafe::queue<unsigned long> C;

        while (!Q.empty())
        {
            unsigned long i = Q.front();
            Q.pop();
            tasks[0].emplace_back(threadpools[1].enqueue([&, i]() {
                vector<future<void>> tasks;

                for (unsigned long p = npsum_[i]; p < npsum_[i + 1]; p++)
                {
                    tasks.emplace_back(threadpools[2].enqueue([&, p]() {
                        lock_guard<mutex> lock(mutexes[NA_[p]]);
                        if (--marked[NA_[p]] == 0)
                            C.push(NA_[p]);
                    }));
                }

                for (auto &t : tasks)
                    t.wait();
            }));
        }

        for (auto &t : tasks[0])
            t.wait();

        while (!C.empty())
        {
            unsigned long p = C.pop().value();
            Q.push(p);

            tasks[1].emplace_back(threadpools[0].enqueue([&, p]() {
                unsigned long min = LONG_MAX;
                for (unsigned long n = IA_[p]; n < IA_[p + 1]; n++)
                    if (innerRank_tmp[JA_[n]] < min)
                        min = innerRank_tmp[JA_[n]];

                if (min < innerRank_tmp[p])
                    innerRank_tmp[p] = min;
            }));
        }

        for (auto &t : tasks[1])
            t.wait();
    }

    outerRank = move(postorder);
    innerRank = move(innerRank_tmp);
}

DAG::DT DAG::toDT()
{
    vector<unsigned long> IA, JA;
    queue<unsigned long> Q;
    vector<vector<unsigned long>> path(V_);
    vector<mutex> mutexes(V_);
    vector<unsigned long> parents(V_);

    vector<unsigned long> np(V_);
    for (unsigned long i = 0; i < V_; i++)
        np[i] = npsum_[i + 1] - npsum_[i];

    for (unsigned long i = 0; i < V_; i++)
        if (isRoot(i))
        {
            Q.push(i);
            parents[i] = i;
            path[i].emplace_back(i);
        }

    while (!Q.empty())
    {
        threadsafe::queue<unsigned long> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpools[0].enqueue([&, p] {
                vector<future<void>> tasks;

                for (unsigned long int i = IA_[p]; i < IA_[p + 1]; i++)
                    tasks.emplace_back(threadpools[1].enqueue([&, p, i](unsigned long n) {
                        vector<future<bool>> swapTasks;
                        lock_guard<mutex> lock(mutexes[n]);
                        bool get_res, new_res, temp_res[PARAL];
                        unsigned long stop = LONG_MAX, g = 0;
                        unsigned long endpos[PARAL];

                        vector<unsigned long> pri = path[p];
                        vector<unsigned long> &qri = path[n];

                        if (IA_[p + 1] - IA_[p] > 1)
                            pri.emplace_back(i);

                        for (int c = 0; c < PARAL; c++)
                        {
                            swapTasks.emplace_back(threadpools[3].enqueue([&, c](const vector<unsigned long> &a, const vector<unsigned long> &b) {
                                endpos[c] = c;
                                if (b.empty())
                                    return true;

                                for (auto ita = a.begin() + c, itb = b.begin() + c; ita < a.end() && itb < b.end(); ita += PARAL, itb += PARAL)
                                {
                                    if (*ita < *itb)
                                        return true;

                                    if (*ita > *itb)
                                        return false;
                                    endpos[c] += PARAL;
                                }

                                return false;
                            },
                                                                          pri, qri));
                        }

                        for (auto &st : swapTasks)
                        {
                            temp_res[g] = st.get();
                            g++;
                        }

                        for (unsigned long c = 0; c < PARAL; c++)
                        {
                            if (endpos[c] < stop)
                            {
                                new_res = temp_res[c];
                                stop = endpos[c];
                            }
                        }

                        if (new_res)
                        {
                            parents[n] = p;
                            qri = pri;
                        }

                        if (--np[n] == 0)
                            P.push(n);
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

    // Caclulating IA and JA of associated directed tree
    vector<unsigned long> occ(V_, 0);
    unsigned long pos = 0;
    for (unsigned long i = 0; i < V_; i++)
    {
        IA.emplace_back(pos);
        for (unsigned long j = IA_[i]; j < IA_[i + 1]; j++)
            if (parents[JA_[j]] == i && ++occ[JA_[j]] == 1)
            {
                JA.emplace_back(JA_[j]);
                pos++;
            }
    }
    IA.emplace_back(pos);

    return DT(V_, move(IA), move(JA), move(parents));
}

void DAG::DT::computeNodeSizeAndPresum(vector<unsigned long> &nodeSize, vector<unsigned long> &presum)
{
    vector<unsigned long> nodeSize_tmp(V_, 1), presum_tmp(V_, 0), marked(V_);
    vector<mutex> mutexes(V_);
    queue<unsigned long> Q;

    for (unsigned long i = 0; i < V_; i++)
        if ((marked[i] = IA_[i + 1] - IA_[i]) == 0)
            Q.push(i);

    while (!Q.empty())
    {
        threadsafe::queue<unsigned long> C;
        vector<future<void>> tasks[2];

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks[0].emplace_back(threadpools[0].enqueue([&](unsigned long p) { // tasks[0].emplace_back(thread([&](node p) {
                lock_guard<mutex> lock(mutexes[p]);
                if (--marked[p] == 0)
                    C.push(p);
            },
                                                         parents_[p]));
        }
        for (auto &t : tasks[0])
            t.wait(); // t.join();

        while (!C.empty())
        {
            unsigned long p = C.pop().value();
            Q.push(p);

            tasks[1].emplace_back(threadpools[0].enqueue([&, p] { // tasks[1].emplace_back(thread([&, p] {
                unsigned long sub = 0;
                for (unsigned long i = IA_[p], j = 0; i < IA_[p + 1]; i++)
                {
                    sub += nodeSize_tmp[JA_[i]];
                    if (j++ > 0)
                        presum_tmp[JA_[i]] = presum_tmp[JA_[i]] + presum_tmp[JA_[i - 1]] + nodeSize_tmp[JA_[i - 1]];
                }

                nodeSize_tmp[p] += sub;
            }));
        }

        for (auto &t : tasks[1])
            t.wait(); // t.join();
    }
    // presum for multiple roots
    unsigned long i, j;
    for (i = 0; i < V_ && !isRoot(i); i++)
        ;
    for (j = i, i = i + 1; i < V_; i++)
        if (isRoot(i))
        {
            presum_tmp[i] = presum_tmp[i] + presum_tmp[j] + nodeSize_tmp[j];
            j = i;
        }

    nodeSize = move(nodeSize_tmp);
    presum = move(presum_tmp);
}

void DAG::DT::parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder)
{
    vector<unsigned long> preorder_tmp(V_, 0), postorder_tmp(V_, 0);
    unsigned long depth = 0;
    queue<unsigned long> Q;

    vector<unsigned long> nodeSize, presum;
    computeNodeSizeAndPresum(nodeSize, presum);

    for (unsigned long i = 0; i < V_; i++)
        if (isRoot(i))
        {
            Q.push(i);
            preorder_tmp[i] = presum[i];
            postorder_tmp[i] = presum[i];
        }

    while (!Q.empty())
    {
        threadsafe::queue<unsigned long> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpools[1].enqueue([&, p] { // tasks.emplace_back(thread([&, p] {
                unsigned long pre = preorder_tmp[p], post = postorder_tmp[p];
                vector<future<void>> tasks;
                // vector<thread> tasks;

                for (unsigned long i = IA_[p]; i < IA_[p + 1]; i++)
                    tasks.emplace_back(threadpools[2].enqueue([&, pre, post](unsigned long i) { // tasks.emplace_back(thread([&, pre, post](node i) {
                        preorder_tmp[i] = pre + presum[i];
                        postorder_tmp[i] = post + presum[i];

                        P.push(i);
                    },
                                                              JA_[i]));

                for (auto &t : tasks)
                    t.wait(); // t.join();

                preorder_tmp[p] = pre + depth;
                postorder_tmp[p] = post + nodeSize[p] - 1;
            }));
        }

        for (auto &t : tasks)
            t.wait(); // t.join();

        P.swap(Q);
        depth++;
    }

    preorder = move(preorder_tmp);
    postorder = move(postorder_tmp);
}