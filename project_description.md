# Project 1: DFS implementation through parallel BFS in Directed Graphs
The project core idea is to implement a parallel DFS implementation as a sequence of 4 BFS, to compute inner and outer rank for each vertex of a given DAG 
The 4 BFS compute each one a different task, and they had to be executed sequentially, because each one uses data computed by the previous one: 
* the first BFS proceeds top-down generates a DT associated to the given DAG 
* the second BFS proceeds bottom-up on the new DT and computes graph subsize for each vertex
* the third BFS proceeds top-down on the new DT and computes preorder and post order for each vertex
* the fourth BFS proceeds bottom-up on the original DAG and computes inner and outer rank


# Classes and data structures 
We implemented the algorithm in C++ language. 
The main class is the __DAG__ ones, which contains: 
* vectors _IA_ e _JA_ for the CSR representation of the adjacency matrix 
* vectors _npsum_ and _NA_ for the CSC representation 
* a class _DT_ to store the associated DT produced by the first BFS
__DT__ class is similar to the DAG one, but has no CSC representation because in a DT each vertex has at most one parent, therefore a simpler _parents_ vector has been used. 
We chose a CSR/CSC representation compared to the standard adjacency matrix to reduce memory usage, but we needed both CSR and CSC because for the first BFS we have to proceed top-down (needing the set of children of each vertex) and for the last one we have to proceed bottom-up (needing the set of parents). 
All numerical data (indexes, sums and so on) are unsigned long, due to the possible big size of the input graphs. 

# Functions and algorithms 
All four BFS are organized in this way: 
* the _labeling_ function called in _main.cpp_ is a wrapper function which calls the _parallelDFS_ ones and then computes inner and outer rank labels for each vertex 
* the _parallelDFS_ function is a wrapper which calls the function for the DAG-to-DT conversion (_toDT_) and then calls the _parallelDFS_ on the associated DT. This ones calls _computeNodeSizeAndPresum_ (2nd BFS) and then computes pre and post order (3rd BFS)

The first BFS generates the associated DT by comparing multiple possible paths for each node, and picking the one that comes first in the graph (following the lexicographic order). The comparison is performed by _swapPath_ function. A path pruning strategy has been adopted (following the one proposed in the given paper in paragraph _4.2.1_) avoiding the store of vertex which cannot provide splits in the path. 

In the dag.cpp file, there is also a standard recursive DFS implementation which we used to perform comparison with our parallel implementation and to prove algorithms correctness. 

# Synchronization
Because each BFS needs data produced by the preceding one and that they proceed in an alternate way (top-down and bottom-up) is impossible to provide synchronization among them but only inside them. 

Inside the BFSs, we used a multithread approach each time a task has to be performed for each parent or each child of a node, or multiple elements in a queue. 
To provide protection and managing concurrent access on data we used: 
* a threadsafe_queue instead of a standard queue, which provides protection (with a mutex) from concurrent push or pop actions. 
* a vector of mutex (one for each vertex) to protect data of each vertex, locking it before any multithread modification. 
* a threadpool implementation to limit the number of running threads. 

The threadpool is needed because in large graphs the number of generated threads is huge and the time needed for context switching in the CPU erases any advantage given by the parallel approach. Therefore the number of active threads is limited by the number of threads supported by the CPU which program is running on. For the threadpool implementation, we used tasks instead of threads, and a queue to manage active tasks. 
When there two levels of nested task we used two threadpools, each one limited on the half of threads supported by the CPU. Even if this option could not be the best one, because we suppose that the number of thread in the two levels is similar, using only one threadpool wasn't possible because it can produce deadlock (e.g. scheduling only outer level threads which have to wait for inner level threads not in execution). 

