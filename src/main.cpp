#define N_ARGS 2

#include <iostream>
#include <fstream>

#include "dag.hpp"

#include "spdlog/spdlog.h"

using namespace std;

void usage(const char *);

int main(int argc, const char *argv[])
{
    if (argc < N_ARGS)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    spdlog::set_level(spdlog::level::info);

    DAG dag(argv[0]);
    unsigned long V = dag.getV();
    vector<unsigned long> innerRank, outerRank;

    dag.labeling(outerRank, innerRank);

    ofstream outfile(argv[1], ios_base::out | ios_base::trunc);

    outfile << V << endl;
    for (unsigned long i = 0; i < V; i++)
        outfile << i << ": " << outerRank[i] << " " << innerRank[i] << endl;
    outfile.close();

    return 0;
}

void usage(const char *progName)
{
    cout << "usage: ";
    cout << progName << " file1 file2" << endl
         << endl;
}