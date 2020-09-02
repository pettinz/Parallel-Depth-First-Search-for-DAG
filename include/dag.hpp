#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include "csr.hpp"
#include "node.h"

using namespace std;

class DAG
{
    CSR csr;
    unsigned long size;
    unsigned long cp;
    vector<node> np;

public:
    DAG(const string &);

    const vector<node> &get_np() const { return np; }
    const unsigned long get_size() const { return size; }

    const CSR &get_csr() const { return csr; }
};

#endif // _DAG_HPP