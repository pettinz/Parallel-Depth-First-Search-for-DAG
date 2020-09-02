#ifndef _DT_HPP
#define _DT_HPP

#include <vector>
#include <mutex>

#include "csr.hpp"
#include "dag.hpp"
#include "node.h"

using namespace std;

class DT
{
    unsigned long size;
    vector<node> parent;
    CSR csr;
    node root;
    vector<mutex> mutexes;

public:
    DT(const DAG &);

    const vector<node> &getParent() const { return parent; }
};

#endif //_DT_HPP