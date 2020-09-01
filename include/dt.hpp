#ifndef _DT_HPP
#define _DT_HPP

#include <vector>

#include "csr.hpp"
#include "dag.hpp"

using namespace std;

class DT {
    unsigned long size;
    vector<unsigned long> parent;
    CSR csr;
    unsigned long root;
public:
    static DT fromDAG(const DAG &);
};

#endif //_DT_HPP