# Project 1: DFS implementation through parallel BFS in Directed Graphs

The aim of the project is to assign a label to each vertex of a DAG by using a parallel implementation of the depth-first search visit as a sequence of 3 BFS (and another one to assign the labels).

Each BFS compute a different task, and they had to be executed sequentially, because each one uses data computed by the previous one:

1. the first BFS proceeds top-down generating a DT associated to the given DAG
2. the second BFS proceeds bottom-up on the new DT and computes graph subsize for each vertex
3. the third BFS proceeds top-down on the new DT and computes preorder and post order for each vertex
4. the fourth BFS proceeds bottom-up on the original DAG and computes inner and outer rank (the label to assign to each vertex)

## Classes and data structures

We implemented the algorithm in C++ language.
The main class is **DAG**, which contains:

- vectors _IA_ e _JA_ for the CSR representation of the adjacency matrix
- vectors _npsum_ and _NA_ for the CSC representation
- a class **DT** to store the associated DT computed by the first BFS. It is similar to the DAG one, but has no CSC representation because in a DT each vertex has at most one parent, therefore a simpler _parents_ vector has been used.

We chose a CSR/CSC representation compared to the standard adjacency matrix to reduce memory usage, but we needed both CSR and CSC because for the first BFS we have to proceed top-down (needing the set of children of each vertex) and for the last one we have to proceed bottom-up (needing the set of parents).
All numerical data (indexes, sums and so on) are unsigned long, due to the possible big size of the input graphs.

## Functions and algorithms

All four BFS are organized in this way:

- `DAG::labeling` method is a wrapper which calls `DAG::parallelDFS` to get pre- and post-order times to compute inner and outer rank labels for each vertex
- `DAG::parallelDFS` is a wrapper which performs the three parallel BFS in sequeunce. In order:
  1. it calls `DAG::toDT` to get the DAG associated DT
  2. it calls `DT::computeNodeSizeAndPresum` on the associated DT
  3. Computes pre- and post-order times on the basis of the achieved results and a top-down BFS on the associated DT.

The first BFS generates the associated DT by comparing multiple possible paths for each node, and picking the one that comes first in the graph (following the lexicographic order). The comparison is performed by the `DAG::swapPath` private method. A path pruning strategy has been adopted (following the one proposed in the given paper in paragraph _4.2.1_) avoiding the store of vertex which cannot provide splits in the path.

The DAG class provides also a standard recursive DFS implementation (`DAG::DFS`) which we used to perform comparison with our parallel implementation and to prove algorithms correctness.

## Synchronization

Because each BFS needs data produced by the preceding one and they proceed in an alternate way (top-down and bottom-up), it is impossible to provide synchronization among them but only inside them.

Inside each BFS, we used a multi-thread approach each time a task has to be performed for each parent or each child of a node, or multiple elements in a queue.
To provide protection and managing concurrent access on data we used:

- a threadsafe queue instead of a standard queue, which provides protection (with a mutex) from concurrent push or pop actions.
- a vector of mutexes (one for each vertex) to protect data of each vertex, locking it before any multithread modification.
- a threadpool implementation to limit the number of running threads.

The threadpool is needed because in large graphs the number of generated threads is huge and the time needed for context switching in the CPU erases any advantage given by the parallel approach. Therefore the number of active threads is limited by the number of threads supported by the CPU which program is running on. For the threadpool implementation, we used tasks instead of threads, and a queue to manage active tasks.

In case of "nested tasks", we used two threadpools, each one limited on the half of threads supported by the CPU, supposing the number of thread in the two levels is similar. Even if this option could not be the best one, using only one threadpool wasn't possible because it can produce deadlock scheduling only outer level threads which have to wait for inner level threads not in execution.
