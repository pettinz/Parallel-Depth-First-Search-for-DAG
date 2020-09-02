#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include "csr.hpp"
<<<<<<< Updated upstream
#include "node.h"
=======
>>>>>>> Stashed changes

using namespace std;

class DAG
{
    CSR csr;
<<<<<<< Updated upstream
    unsigned long size;
    unsigned long cp;
    vector<node> np;
=======
    unsigned long size; 
    unsigned long cp;
    vector<unsigned long> np;
>>>>>>> Stashed changes

public:
    DAG(const string &);

    const vector<node> &get_np() const { return np; }
    const unsigned long get_size() const { return size; }

<<<<<<< Updated upstream
    const CSR &get_csr() const { return csr; }
=======
    vector<unsigned long> get_np() const { return np; }
    const unsigned long get_size() const { return size; }
    
    const CSR &get_csr() const { return csr; }

    void subgraph_size_init();
>>>>>>> Stashed changes
};

#endif // _DAG_HPP