#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include "node.h"

#include <queue>

using namespace std;

class DAG
{
    vector<unsigned long> IA_, JA_, NA_;
    unsigned long V_;
    unsigned long cp_;
    vector<node> np_, npsum_;
    vector<node> roots_;

private:
    class DT
    {
        unsigned long V_;
        vector<unsigned long> IA_, JA_, parents_, roots_;

    public:
        DT(unsigned long V, vector<unsigned long> IA, vector<unsigned long> JA, vector<unsigned long> parents, vector<unsigned long> roots) : V_(V), IA_(move(IA)), JA_(move(JA)), parents_(move(parents)), roots_(move(roots)) {}

        void computeNodeSizeAndPresum(vector<unsigned long> &nodeSize, vector<unsigned long> &presum);

        const vector<unsigned long> &getIA() const { return IA_; }
        const vector<unsigned long> &getJA() const { return JA_; }
        const vector<unsigned long> &getParents() const { return parents_; }
        const vector<unsigned long> &getRoots() const { return roots_; }
        bool isRoot(unsigned long i) { return parents_[i] == i; }

        void parallelDFS(vector<node> &preorder, vector<node> &postorder);
    };

    bool swapPath(const vector<node> &a, const vector<node> &b);
    void DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);

    DT toDT();

public:
    DAG(const string &);
    void DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);
    void ParallelDFS(vector<node> &preorder, vector<node> &postorder);
    void labeling(vector<unsigned long> &postorder, vector<unsigned long> &innerRank);

    const vector<node> &get_np() const { return np_; }
    const unsigned long getV() const { return V_; }

    const vector<unsigned long> &getIA() const { return IA_; }
    const vector<unsigned long> &getJA() const { return JA_; }
};

#endif // _DAG_HPP