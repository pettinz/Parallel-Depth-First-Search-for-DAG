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
    bool swapPath(const vector<node> &a, const vector<node> &b);
    void DAG::DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);
    
public:
    DAG(const string &);
    void DAG::DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);
    void ParallelDFS(vector<node> &preorder, vector<node> &postorder);
    void ParallelDFSUtil1(vector<node> &dtIA, vector<node> &dtJA, vector<node> &dtParents);
    void ParallelDFSUtil2(const vector<node> &dtIA, const vector<node> &dtJA, const vector<node> &dtParents, vector<unsigned long> &nodeSize, vector<unsigned long> &presum);
    void ParallelDFSUtil3(const vector<node> &dtIA, const vector<node> &dtJA, const vector<node> &dtParents, const vector<unsigned long> &nodeSize, const vector<unsigned long> &presum, vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    void DAG::labelingUtil(vector<unsigned long> &postorder, vector<unsigned long> &innerRank);


    const vector<node> &get_np() const { return np_; }
    const unsigned long getV() const { return V_; }

    const vector<unsigned long> &getIA() const { return IA_; }
    const vector<unsigned long> &getJA() const { return JA_; }
};

#endif // _DAG_HPP