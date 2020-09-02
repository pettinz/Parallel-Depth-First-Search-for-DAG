#define PARALLEL_LEAF_SEARCH 0

#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>

#include "dag.hpp"

#include "spdlog/spdlog.h"

DAG::DAG(const string &fileName)
{
    ifstream infile(fileName);
    if (!infile.is_open())
    {
        spdlog::critical("Error opening file {}", fileName);
        abort();
    }

    string line;
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

    infile.close();
}