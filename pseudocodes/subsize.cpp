vector<unsigned long> subgraph_size;
void compute_subgraph_size(); 

void DT::compute_subgraph_size () { 
    subgraph_size = vector<unsigned long>(size, 1);
    vector<unsigned long> presum = vector<unsigned long>(size, 0);
    vector<unsigned long> to_be_marked = vector<unsigned long>(size);
    queue<node> Q; 
 

    for (int i = 0; i<size; i++) {
        if (IA[i+1] - IA[i] == 0) 
            Q.push(i); 
        to_be_marked[i] = IA[i+1] - IA[i]; 
    } 

    while (!Q.empty())
    {
        threadsafe::queue<unsigned long> C;
        vector<future<void>> tasksQ;
        vector<future<void>> tasksC;

        while (!Q.empty()) {
            unsigned long i = Q.front();
            Q.pop();

            tasksQ.emplace_back(threadpool.enqueue([&, i] {

                unsigned long p = parents[i]; 
                lock_guard<mutex> lock(mutexes[p]);

                if (--to_be_marked[p] == 0)
                    C.push(i);      
            }));

        }

        for (auto &t : tasksQ)
            t.wait();

        // NOPE SERVE UN ITERATORE! 
        while (!C.empty()) {
            unsigned long p = C.front();
            C.pop();

            tasksC.emplace_back(threadpool.enqueue([&, p] {            
                unsigned long sub = 0; 
                unsigned long j = 0; 
                for (unsigned long i = IA[p]; i < IA[p+1]; i++) {
                    sub += subgraph_size[i];
                    if (j>0)
                        presum[i] += subgraph_size[i-1]; 
                    j++; 
                }
                subgraph_size[p] += sub; 
            }));
        }

        for (auto &t : tasksC)
            t.wait();
        
        C.swap(Q);
    } 

}