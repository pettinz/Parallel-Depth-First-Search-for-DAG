#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

using namespace std;

class CSR {
    vector<unsigned long> _IA, _JA;

public:
    vector<unsigned long> getIA();
    vector<unsigned long> getJA();

    void setIA(const vector<unsigned long>&);
    void setJA(const vector<unsigned long>&);
};

class DAG
{
    CSR _csr;
    unsigned long _size; 

public:
    void readFromFile(const string&);

    CSR getCSR();
};

#endif  // _DAG_HPP