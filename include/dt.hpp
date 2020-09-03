#ifndef _DT_HPP
#define _DT_HPP

#include <vector>
#include <mutex>

#include "dag.hpp"
#include "node.h"

using namespace std;

class DT
{
    vector<unsigned long> IA, JA;
    unsigned long size;
    vector<node> parents;
    node root;
    vector<mutex> mutexes;

public:
    explicit DT(const DAG &);

    const vector<node> &getParent() const { return parents; }
    const vector<unsigned long> &getIA() const { return IA; }
    const vector<unsigned long> &getJA() const { return JA; }

    void computeNodeSize(vector<node> &nodeSize, vector<node> &presum);
    void computeOrder(vector<unsigned long> &preOrder, vector<unsigned long> &postOrdedr, const vector<node> &nodeSize, const vector<node> &presum);
};

#endif //_DT_HPP