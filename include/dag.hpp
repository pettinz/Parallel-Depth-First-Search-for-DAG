#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include "node.h"

using namespace std;

class DAG
{
    vector<unsigned long> IA, JA;
    unsigned long size;
    unsigned long cp;
    vector<node> np;

public:
    DAG(const string &);

    const vector<node> &get_np() const { return np; }
    const unsigned long get_size() const { return size; }

    const vector<unsigned long> &getIA() const { return IA; }
    const vector<unsigned long> &getJA() const { return JA; }
};

#endif // _DAG_HPP