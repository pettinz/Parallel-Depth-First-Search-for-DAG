#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

using namespace std;

/**
 * @brief Directed acyclic graph class.
 * CSR/CSC representation is used instead of the adjacency matrix to reduce memory useage
 */
class DAG
{
    unsigned long V_;             /*!< number of graph vertex */
    vector<unsigned long> IA_;    /*!< child cumulative sum vector for the CSR */
    vector<unsigned long> JA_;    /*!< child list vector for the CSR */
    vector<unsigned long> npsum_; /*!< parent cumulative sum vector for the CSC */
    vector<unsigned long> NA_;    /*!< parent list vector for the CSC */

private:
    /**
    * @brief Directed tree class.
    * CSR representation is used, CSC one is substituted with parents vector
    */
    class DT
    {
        unsigned long V_;               /*!< number of graph vertex */
        vector<unsigned long> IA_;      /*!< child cumulative sum vector for the CSR */
        vector<unsigned long> JA_;      /*!< child list vector for the CSR */
        vector<unsigned long> parents_; /*!< contains the parent of each node */
    
    private:
        /**
         * @brief Compute for each node its subgraph size and presum on its left siblings with a bottom-up BFS visit
         * 
         * @param nodeSize will carry the subgraph size of each node (index)
         * @param presum will carry the presum of each node (index)
         */
        void computeNodeSizeAndPresum(vector<unsigned long> &nodeSize, vector<unsigned long> &presum);
        void seqcomputeNodeSizeAndPresum(vector<unsigned long> &nodeSize, vector<unsigned long> &presum);

    public:
        DT(unsigned long V, vector<unsigned long> IA, vector<unsigned long> JA, vector<unsigned long> parents) : V_(V), IA_(move(IA)), JA_(move(JA)), parents_(move(parents)) {}

        /**
         * @brief Get IA vector.
         * 
         * @return const vector<unsigned long>& IA_
         */
        const vector<unsigned long> &getIA() const { return IA_; }
        /**
         * @brief Get JA vector
         * 
         * @return const vector<unsigned long>& JA_
         */
        const vector<unsigned long> &getJA() const { return JA_; }
        /**
         * @brief Get the parents vector
         * 
         * @return const vector<unsigned long>& parents_
         */
        const vector<unsigned long> &getParents() const { return parents_; }

        /**
         * @brief Check if a node is a root.
         * 
         * @param i node to be evaluated
         * @return true, if the evaluated node is a root
         * @return false, otherwise
         */
        bool isRoot(unsigned long i) { return parents_[i] == i; }
        /**
         * @brief Assign two labels (inner and outer rank) to each vertex. It mimics DFS execution in a parallel way
         * 
         * @param outerRank will carry the outer rank of each vertex 
         * @param innerRank will carry the inner rank of each vertex
         */
        void parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
        void seqparallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    };

    /**
     * @brief Swap two path if first one precedes the second one in the graph order
     * 
     * @param a the first path
     * @param b the second path
     * @return true, if the first path precedes the second one in the graph order
     * @return false, otherwise
     */
    bool swapPath(const vector<unsigned long> &a, const vector<unsigned long> &b);
    /**
     * @brief Utility for DFS recursion
     * 
     * @param v current vertex
     * @param pre pre-order time
     * @param post post-order time
     * @param visited carry for each vertex (index) if it has been visited or not
     * @param preorder 
     * @param postorder 
     * @param innerRank 
     * @param outerRank 
     */
    void DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);

    /**
     * @brief Create the DAG associated directed tree following vertex order to choose the path with a top-down BFS visit
     * 
     * @return DT DAG associeated directed tree
     */
    DT toDT();
    DT seqtoDT();

public:
    /**
     * @brief Construct a new DAG object from file.
     * 
     */
    explicit DAG(const string &);

    /**
     * @brief Get the number of vertices.
     * 
     * @return const unsigned long V_
     */
    const unsigned long getV() const { return V_; }
    /**
     * @brief Get IA vector.
     * 
     * @return const vector<unsigned long>& IA_
     */
    const vector<unsigned long> &getIA() const { return IA_; }
    /**
     * @brief Get JA vector.
     * 
     * @return const vector<unsigned long>& JA_
     */
    const vector<unsigned long> &getJA() const { return JA_; }

    /**
     * @brief Check if a vertex is a root.
     * 
     * @param i vertex to be evaluated
     * @return true, if the evaluated node is a root
     * @return false, otherwise
     */
    bool isRoot(unsigned long i) { return npsum_[i] == npsum_[i + 1]; }
    /**
     * @brief Perform a depth-first search visit for the current DAG in a recursive way
     * 
     * @param preorder will carry the pre-order times
     * @param postorder will carry the post-order times
     * @param innerRank 
     * @param outerRank 
     */
    void DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);
    /**
     * @brief Perform a multithreaded depth-first search visit for the current DAG.
     * By means of 3 BFS visits:
     *     1. The first one generates the associated DT
     *     2. The second one computes on the associated DT the subgraph size and presum of each node.
     *     3. The last one computes pre-order and post-order times using the above information
     * 
     * @param preorder will carry the pre-order times
     * @param postorder will carry the post-order times
     */
    void parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    void seqparallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    /**
     * @brief Assign two labels (inner and outer rank) to each vertex.
     * Use preorder and postorder computed by a DFS.
     * 
     * @param outerRank will carry the outer rank of each vertex 
     * @param innerRank will carry the inner rank of each vertex
     */
    void labeling(vector<unsigned long> &outerRank, vector<unsigned long> &innerRank);
    void seqlabeling(vector<unsigned long> &outerRank, vector<unsigned long> &innerRank);
};

#endif // _DAG_HPP