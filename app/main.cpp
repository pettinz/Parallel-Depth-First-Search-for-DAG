#define N_ARGS 3

#include <iostream>
#include <fstream>

#include "dag.hpp"

#include "spdlog/spdlog.h"

using namespace std;

void usage(const char *);

int main(int argc, const char *argv[])
{
    // Set logging level
    spdlog::set_level(spdlog::level::info);

    // Check on args
    if (argc != N_ARGS)
    {
        if (argc < N_ARGS)
            spdlog::critical("Missing arguments.");
        else
            spdlog::critical("Too many arguments.");

        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // START
    unsigned long V;
    vector<unsigned long> innerRank, outerRank;

    spdlog::info("Creating DAG...");
    DAG dag(argv[1]);
    spdlog::info("\tdone.");

    spdlog::info("Computing labels...");
    dag.labeling(outerRank, innerRank);
    spdlog::info("\tdone.");

    ofstream outfile(argv[2], ios_base::out | ios_base::trunc);
    if (!outfile.is_open())
    {
        spdlog::critical("Error creating file {}.", argv[2]);
        abort();
    }

    spdlog::info("Writing labels into {}...", argv[2]);
    V = dag.getV();
    outfile << V << endl;
    for (unsigned long i = 0; i < V; i++)
        outfile << i << ": " << innerRank[i] << " " << outerRank[i] << endl;
    outfile.close();
    spdlog::info("\tdone.");

    return 0;
}

void usage(const char *progName)
{
    cerr << "usage: ";
    cerr << progName << " file1 file2" << endl
         << endl;
}