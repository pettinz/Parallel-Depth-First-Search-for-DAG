#ifndef _CSR_HPP
#define _CSR_HPP

#include <fstream>
#include <sstream>
#include <vector>

#include "spdlog/spdlog.h"

using namespace std;

class CSR
{
    vector<unsigned long> IA, JA;

public:
    CSR() : IA(0), JA(0) {}
    CSR(const vector<unsigned long> &IA, const vector<unsigned long> &JA) : IA(IA), JA(JA) {}

    const vector<unsigned long> &getIA() const { return IA; }
    const vector<unsigned long> &getJA() const { return JA; }
};

#endif // _CSR_HPP
