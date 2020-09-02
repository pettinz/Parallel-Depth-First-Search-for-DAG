#define PARALLEL_LEAF_SEARCH 0

#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"

#include "spdlog/spdlog.h"

DAG::DAG(const string &fileName) : size(0)
{
    ifstream infile(fileName);
    if (!infile.is_open())
    {
        spdlog::critical("Error opening file {}", fileName);
        abort();
    }

    string line;
    vector<unsigned long> IA;
    vector<node> JA;
    unsigned long i, j;
    unsigned long n;

    n = 0;
    IA.emplace_back(n);

    getline(infile, line);
    size = stoi(line);

    np.resize(size);

    while (getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> i;
        iss.seekg(sizeof(char), ios_base::cur);

        while (iss >> j)
        {
            JA.emplace_back(j);
            np[j]++;
            n++;
        }

        IA.emplace_back(n);
    }

    csr = CSR(IA, JA);

    infile.close();
}