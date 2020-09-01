#ifndef _CSR_HPP
#define _CSR_HPP

#include <vector>

using namespace std;

class CSR {
    vector<unsigned long> IA, JA;

public:
    const vector<unsigned long> &getIA() const { return IA; }
    const vector<unsigned long> &getJA() const { return JA; }

    void setIA(const vector<unsigned long> &v) { IA = v; }
    void setJA(const vector<unsigned long> &v) { JA = v; }
};

#endif // _CSR_HPP
