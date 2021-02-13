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

## Project structure

The project is a cross-platform CMake project written in C++ (see [README](README.md) for further details on building, running and/or testing).

The main files are:

- _include/dag.hpp_, the header file, which contains the prototype of the `DAG` class
- _src/dag.cpp_, the source file, which contains the implementation of the `DAG` class
- _main.cpp_, the main source file, which prints in the output file the final labeling of the DAG received as argument

The _lib_ directory contains the following libraries

- _threadpool_, a simple C++11 Thread Pool implementation, provided by [progschj](https://github.com/progschj/ThreadPool)
- _threadsafe_queue_, a thread safe queue implementation, used for operation of push and pop in a queue in a multi-threaded context
- _spdlog_, a very fast C++ logging library, used to provide some logging to the user, provided by [gabime](https://github.com/gabime/spdlog)

The _test_ directory contains the following:

- _gra/_ directory, which contains the graph files
- _include/catch.hpp_, the library used for unit testing
- _benchmark.cpp_, which provieds the execution time for each graph files in the _gra_ directory
- _test.cpp_, which checks the algorithm correctness comparing the results provided by `DAG::parallelDFS` and the results provided by `DAG::DFS`

## Obtained results 
The algorithm correctness has been proven running a comparison with the result obtained with a standard recursive DFS implementation. 
Although the correctness of the algorithm has been proven, the result obtained in term of time are not surprising: the algorithm is slower than the DFS when run on a commonly available CPU. 
In our case test have been performed on an AMD Ryzen 7 3700x CPU with 8 core and 16 hyper-thread, paired with 16 GB of Ram at 3400Mhz on a Linux Ubuntu 20.04 installation.  
Here below we report some of the obtained results, with the program running with 1, 2, 4, 8 and 16 threads in parallel on three different graphs with different dimensions: one graph from "small dense real" folder (*citeseer_sub_10720.gra*), one from "small sparse real" folder (*amaze_dag_uniq.gra*)and the third from "large real" folder (*cit-Patents.scc.gra*)
Threads number has limited changing the default value of "hardware concurrency" for the program and forcing the kernel to schedule it on a subset of available hyper-threads with the command "taskset" to select the usable hyper-threads for the following program. Runlim in version 1.7 has been used to track time and memory usage. 
Runlim *real* output line refers to the real time required, and *space* refers to the amount of main memory used. 
We focused our consideration mainly on small dense and large files since small sparse execution were too fast and runlim was not able to catch samples and report meaningful results but we reported also small sparse screen for completeness. 
Screenshots are reported in order with the increasing number of thread used for each graph.

##### Results on small sparse real graph
<img src="1T_SS.png" width="800"/> \n
<img src="2T_SS.png" width="800"/> \n
<img src="4T_SS.png" width="800"/> \n
<img src="8T_SS.png" width="800"/> \n
<img src="16T_SS.png" width="800"/> 

##### Results on small dense real graph 
<img src="1T_SD.png" width="800"/> \n
<img src="2T_SD.png" width="800"/> \n
<img src="4T_SD.png" width="800"/> \n
<img src="8T_SD.png" width="800"/> \n
<img src="16T_SD.png" width="800"/> 

##### Results on large real graph 
<img src="1T_LR2.png" width="800"/> \n
<img src="2T_LR2.png" width="800"/> \n
<img src="4T_LR2.png" width="800"/> \n
<img src="8T_LR2.png" width="800"/> \n
<img src="16T_LR2.png" width="800"/> 


We can notice that the required time decreased from 1 to 2 and from 2 to 4 running threads, but it start increasing again with 8 or 16 threads. 
This is probably related to the increasing number of context switch with the number of cores and the implementation of the threadpool which pushes in the queue tasks in the order in which they have been created, often pushing threads that have to wait for mutex and that therefore remain in a wait status. 
In a different environment that can better manage parallelization and with an ad-hoc implementation of the threadpool library it would be possible to get better results, improving the threadpool with a check over the related mutex before scheduling a task and swapping out tasks in wait status. 
With big and complex graphs the number of generated task is huge therefore it is very complex managing them efficiently. We also noticed that removing one level of nested task (changing the code from the one proposed in the paper) does not provide any performance improvement probably because even with only one level of threads the algorithm saturates in any case the number of available core in the CPU letting some threads waiting in the threadpool queue. 
The main advantage of this implementation over DFS one is that this parallel algorithm can better handle memory usage: even where the DFS execution crashes due to memory error (probably over the recursion stack) this implementation running on the same amount of memory can complete the execution successfully. From the screenshots above it is possible to see that the amount of memory required doesn't change with the number of threads, because each thread doesn't allocate any new structure. 
