#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"
#include "threadpool.hpp"
#include "threadsafe_queue.hpp"

#include "spdlog/spdlog.h"

ThreadPool threadpools[3] = {ThreadPool(thread::hardware_concurrency()), ThreadPool(thread::hardware_concurrency() / 2), ThreadPool(thread::hardware_concurrency() / 2)};

bool DAG::swapPath(const vector<unsigned long> &a, const vector<unsigned long> &b)
{
    if (b.empty())
        return true;

    if (a[0] < b[0])
        return true;

    for (auto ita = next(a.begin()), itb = next(b.begin()); ita != a.end() && itb != b.end(); ita++, itb++)
    {
        if (JA_[*ita] == JA_[*itb])
            continue;

        if (*ita < *itb)
            return true;

        return false;
    }

    return false;
}

void DAG::DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank)
{
    visited[v] = true;
    preorder[v] = pre++;
    unsigned long min = LONG_MAX;

    for (unsigned long i = IA_[v]; i < IA_[v + 1]; i++){
        if (!visited[JA_[i]])
            DFSUtil(JA_[i], pre, post, visited, preorder, postorder, innerRank, outerRank);
        if (innerRank[JA_[i]] < min)
            min = innerRank[JA_[i]];
        }

    innerRank[v] = min; 
    postorder[v] = post++;
    outerRank[v] = post; 
    if (outerRank[v] < min ) // foglia 
        innerRank[v] = outerRank[v]; 
        
}

void DAG::DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank)
{
    vector<bool> visited(V_, false);
    vector<unsigned long> preorder_tmp(V_), postorder_tmp(V_), innerRank_tmp(V_), outerRank_tmp(V_);
    unsigned long pre = 0, post = 0;

    //for (node i = 0; i < V_; i++)
    for (auto i : roots_)
        if (visited[i] == false) // && np_[i] == 0)
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
    vector <unsigned long> KA_; 

    n = 0;
    k = 0; 
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
            KA_.emplace_back(k);
            np_[j]++;
            n++;
        }
        k++;
        IA_.emplace_back(n);
    }

    unsigned long ns = 0; 
    npsum_.emplace_back(ns);
    // Finding roots
    for (node i = 0; i < V_; i++) {
        if (np_[i] == 0)
            roots_.emplace_back(i);
        ns += np_[i];
        npsum_.emplace_back(ns); 
    }

        // building NA 
    NA_ = vector<unsigned long> (JA_.size());
    unsigned long el;
    vector<unsigned long> cc = vector<unsigned long> (V_, 0); 
    for (unsigned long j = 0; j<JA_.size(); j++ ){ 
        el = JA_[j]; 
        unsigned long pos = npsum_[el] + cc[el]; 
        cc[el]++; 
        NA_[pos] = KA_[j] ;
    }


    infile.close();
}

void DAG::ParallelDFS(vector<node> &preorder, vector<node> &postorder)
{
    vector<node> IA, JA, parents;
    ParallelDFSUtil1(IA, JA, parents);

    vector<unsigned long> subgraphSize, presum;
    ParallelDFSUtil2(IA, JA, parents, subgraphSize, presum);

    ParallelDFSUtil3(IA, JA, parents, subgraphSize, presum, preorder, postorder);
}

void DAG::ParallelDFSUtil1(vector<node> &dtIA, vector<node> &dtJA, vector<node> &dtParents)
{
    vector<node> IA, JA;
    queue<node> Q;
    vector<vector<node>> path(V_);
    vector<mutex> mutexes(V_);
    vector<node> parents(V_);
    vector<node> np = np_;

    for (auto n : roots_)
    {
        Q.push(n);
        parents[n] = n;
        path[n].emplace_back(n);
    }

    while (!Q.empty())
    {
        threadsafe::queue<node> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpools[0].enqueue([&, p] {
                vector<future<void>> tasks;

                for (unsigned long int i = IA_[p]; i < IA_[p + 1]; i++)
                    tasks.emplace_back(threadpools[1].enqueue([&, p, i](node n) {
                        lock_guard<mutex> lock(mutexes[n]);

                        vector<node> pri = path[p];
                        vector<node> &qri = path[n];

                        pri.emplace_back(i);

                        if (swapPath(pri, qri))
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
    for (node i = 0; i < V_; i++)
    {
        IA.emplace_back(pos);
        for (node j = IA_[i]; j < IA_[i + 1]; j++)
            if (parents[JA_[j]] == i && ++occ[JA_[j]] == 1)
            {
                JA.emplace_back(JA_[j]);
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

    for (node i = 0; i < V_; i++)
        if ((marked[i] = dtIA[i + 1] - dtIA[i]) == 0)
            Q.push(i);

    while (!Q.empty())
    {
        threadsafe::queue<node> C;
        vector<future<void>> tasks[2];

        while (!Q.empty())
        {
            unsigned long p = Q.front();
            Q.pop();

            tasks[0].emplace_back(threadpools[0].enqueue([&](node p) { // tasks[0].emplace_back(thread([&](node p) {
                lock_guard<mutex> lock(mutexes[p]);
                if (--marked[p] == 0)
                    C.push(p);
            },
                                                         dtParents[p]));
        }
        for (auto &t : tasks[0])
            t.wait(); // t.join();

        while (!C.empty())
        {
            node p = C.pop().value();
            Q.push(p);

            tasks[1].emplace_back(threadpools[0].enqueue([&, p] { // tasks[1].emplace_back(thread([&, p] {
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
            t.wait(); // t.join();
    }
    // presum for multiple roots
    for (auto i = next(roots_.begin()); i < roots_.end(); i++)
        presum_tmp[*i] = presum_tmp[*i] + presum_tmp[*prev(i)] + nodeSize_tmp[*prev(i)];

    nodeSize = move(nodeSize_tmp);
    presum = move(presum_tmp);
}

void DAG::ParallelDFSUtil3(const vector<node> &dtIA, const vector<node> &dtJA, const vector<node> &dtParents, const vector<unsigned long> &nodeSize, const vector<unsigned long> &presum, vector<unsigned long> &preorder, vector<unsigned long> &postorder)
{
    vector<unsigned long> preorder_tmp(V_, 0), postorder_tmp(V_, 0);
    unsigned long depth = 0;
    queue<node> Q;

    for (auto n : roots_)
    {
        Q.push(n);
        preorder_tmp[n] = presum[n];
        postorder_tmp[n] = presum[n];
    }

    while (!Q.empty())
    {
        threadsafe::queue<node> P;
        vector<future<void>> tasks;

        while (!Q.empty())
        {
            node p = Q.front();
            Q.pop();

            tasks.emplace_back(threadpools[1].enqueue([&, p] { // tasks.emplace_back(thread([&, p] {
                unsigned long pre = preorder_tmp[p], post = postorder_tmp[p];
                vector<future<void>> tasks;
                // vector<thread> tasks;

                for (unsigned long i = dtIA[p]; i < dtIA[p + 1]; i++)
                    tasks.emplace_back(threadpools[2].enqueue([&, pre, post](node i) { // tasks.emplace_back(thread([&, pre, post](node i) {
                        preorder_tmp[i] = pre + presum[i];
                        postorder_tmp[i] = post + presum[i];

                        P.push(i);
                    },
                                                              dtJA[i]));

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

void DAG::labelingUtil(vector<unsigned long> &postorder, vector<unsigned long> &innerRank)
{
    vector<node> innerRank_tmp, marked;
    queue<node> Q;
    vector<mutex> mutexes(V_);

    for (node i = 0; i < V_; i++)
    {
        if (IA_[i + 1] == IA_[i])
            Q.push(i);

        marked.emplace_back(IA_[i + 1] - IA_[i]);
        innerRank_tmp.emplace_back(++postorder[i]);
    }

    while (!Q.empty())
    {
        //vector<thread> tasks[2];
        vector<future<void>> tasks1, tasks2;
        threadsafe::queue<node> C;

        while (!Q.empty())
        {
            node i = Q.front();
            Q.pop();
            tasks1.emplace_back(threadpools[1].enqueue([&, i](){
                vector<future<void>> tasks;
            
                for (unsigned long p = npsum_[i]; p < npsum_[i + 1]; p++) {
                    tasks.emplace_back(threadpools[2].enqueue([&, p]() {
                        lock_guard<mutex> lock(mutexes[NA_[p]]);
                        if (--marked[NA_[p]] == 0) 
                            C.push(NA_[p]);
                        
                    } ));  
                }

                for (auto &t : tasks)
                    t.wait();
            }));
        } 

        for (auto &t : tasks1)
            t.wait();


        while (!C.empty()) 
        {
            node p = C.pop().value();
            Q.push(p);

            tasks2.emplace_back(threadpools[0].enqueue([&, p]() {
                unsigned long min = LONG_MAX;
                for (unsigned long n = IA_[p]; n < IA_[p + 1]; n++) {
                    if (innerRank_tmp[JA_[n]] < min){
                        min = innerRank_tmp[JA_[n]];
                    }
                }
                if (min < innerRank_tmp[p]) {
                    innerRank_tmp[p] = min;
                }
            }));
        }

        for (auto &t : tasks2)
            t.wait();

        
    }

    innerRank = move(innerRank_tmp);
}