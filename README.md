# Parallel Depth-First Search for Directed Acyclic Graphs

## Requirements

- `spdlog` (https://github.com/gabime/spdlog) Very fast, header-only/compiled, C++ logging library
- `Catch2` (https://github.com/catchorg/Catch2) Catch2 is a multi-paradigm test framework for C++

## Install

```bash
$ git clone https://github.com/pettinz/Parallel-Depth-First-Search-for-DAG.git && cd Parallel-Depth-First-Search-for-DAG
$ mkdir build && cd build
$ cmake .. && make -j
```

## Run

In build directory

```bash
./main file1 file2
```

where `file1` is the file containing the DAG (with a specific and given format) and `file2` is the output file, storing the labeling.
