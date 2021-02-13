# Parallel Depth-First Search for Directed Acyclic Graphs

## Install

```bash
$ git clone https://github.com/pettinz/Parallel-Depth-First-Search-for-DAG.git && cd Parallel-Depth-First-Search-for-DAG
$ cmake -Bbuild
$ cmake --build build
```

If you are on Windows,

```bash
$ cmake --build build --config release
```

## Docs

After building

```bash
$ cmake --build build --target docs
```

Docs are inside `build/docs/html` in html format.

## Run

In build directory

```bash
./build/app/main file1 file2
```

where `file1` is the file containing the DAG (with a specific and given format) and `file2` is the output file, storing the labeling.

## Test

After building, test are inside `build/test`. Tests include also a banchmark application to compare the parallel DFS timing with the sequetial one.

```bash
$ cmake --build build --target test
```
