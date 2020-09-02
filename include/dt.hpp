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
    DT(const DAG &);

    const vector<node> &getParent() const { return parents; }
};

#endif //_DT_HPP