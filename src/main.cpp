#define N_ARGS 2

#include <iostream>

#include "dag.hpp"
#include "dt.hpp"

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

    DAG dag(argv[1]);
    DT dt(dag);

    spdlog::info("DT generated");

    return 0;
}

void usage(const char *progName)
{
    cout << "usage: ";
    cout << progName << " file1 file2" << endl
         << endl;
}