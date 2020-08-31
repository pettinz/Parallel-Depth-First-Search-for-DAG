#include <fstream>
#include <sstream>

#include "dag.hpp"
#include "spdlog/spdlog.h"

void CSR::setIA(const vector<unsigned long> &v) { _IA = v; }
void CSR::setJA(const vector<unsigned long> &v) { _JA = v; }
vector<unsigned long> CSR::getIA() { return _IA; }
vector<unsigned long> CSR::getJA() { return _JA; }

void DAG::readFromFile(const string &fileName)
{
    ifstream infile(fileName);
    if (!infile.is_open())
    {
        spdlog::critical("Error opening file {}", fileName);
        abort();
    }

    string line;
    vector<unsigned long> IA, JA;
    unsigned long i, j;
    unsigned long n;

    n = 0;
    IA.emplace_back(n);

    getline(infile, line);
    _size = stoi(line);

    while (getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> i;
        iss.seekg(sizeof(char), ios_base::cur);

        while (iss >> j)
        {
            JA.emplace_back(j);
            n++;
        }

        IA.emplace_back(n);
    }

    _csr.setIA(IA);
    _csr.setJA(JA);

    infile.close();
}

CSR DAG::getCSR() { return _csr; }