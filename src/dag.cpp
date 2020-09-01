#define PARALLEL_LEAF_SEARCH 0

#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"

#include "spdlog/spdlog.h"

DAG DAG::fromFile(const string &fileName)
{
    ifstream infile(fileName);
    if (!infile.is_open())
    {
        spdlog::critical("Error opening file {}", fileName);
        abort();
    }

    DAG dag;
    string line;
    vector<unsigned long> IA, JA;
    unsigned long i, j;
    unsigned long n;

    n = 0;
    IA.emplace_back(n);

    getline(infile, line);
    dag.size = stoi(line);

    dag.np.resize(dag.size);

    while (getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> i;
        iss.seekg(sizeof(char), ios_base::cur);

        while (iss >> j)
        {
            JA.emplace_back(j);
            dag.np[j]++;
            n++;
        }

        IA.emplace_back(n);
    }

    dag.csr.setIA(IA);
    dag.csr.setJA(JA);

    infile.close();

    return dag;
}