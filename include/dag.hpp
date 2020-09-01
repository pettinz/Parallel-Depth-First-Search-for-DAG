#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include "csr.hpp"

using namespace std;

class DAG
{
    CSR csr;
    unsigned long size; 
    unsigned long cp;
    vector<unsigned long> np;

public:
    static DAG fromFile(const string&);

    vector<unsigned long> get_np() const { return np; }
    const unsigned long get_size() const { return size; }
    
    const CSR &get_csr() const { return csr; }
};

#endif  // _DAG_HPP