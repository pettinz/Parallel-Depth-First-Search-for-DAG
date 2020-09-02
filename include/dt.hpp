#ifndef _DT_HPP
#define _DT_HPP

#include <vector>
<<<<<<< Updated upstream
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
=======

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
>>>>>>> Stashed changes
};

#endif //_DT_HPP