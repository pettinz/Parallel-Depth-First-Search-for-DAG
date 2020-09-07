#ifndef _DAG_HPP
#define _DAG_HPP

#include <string>
#include <vector>

#include <queue>

using namespace std;

/**
 * @brief Directed acyclic graph class.
 * 
 */
class DAG
{
    vector<unsigned long> IA_, JA_;
    vector<unsigned long> npsum_, NA_;
    unsigned long V_;

private:
    class DT
    {
        unsigned long V_;
        vector<unsigned long> IA_, JA_, parents_;

    private:
        /**
         * @brief Compute for each node its size and presum
         * 
         * @param nodeSize vector containing the size of each node (index)
         * @param presum vector containing the presum of each node (index)
         */
        void computeNodeSizeAndPresum(vector<unsigned long> &nodeSize, vector<unsigned long> &presum);

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
         * @return true if the evaluated node is a root
         * @return false otherwise
         */
        bool isRoot(unsigned long i) { return parents_[i] == i; }
        /**
         * @brief Assign a label to each vertex.
         * 
         * @param outerRank 
         * @param innerRank 
         */
        void parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    };

    /**
     * @brief 
     * 
     * @param a 
     * @param b 
     * @return true 
     * @return false otherwise
     */
    bool swapPath(const vector<unsigned long> &a, const vector<unsigned long> &b);
    /**
     * @brief Utility for DFS recursion
     * 
     * @param v current vertex
     * @param pre pre-order time
     * @param post post-order time
     * @param visited contains for each vertex (index) if it has been visited or not
     * @param preorder 
     * @param postorder 
     * @param innerRank 
     * @param outerRank 
     */
    void DFSUtil(unsigned long v, unsigned long &pre, unsigned long &post, vector<bool> &visited, vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);

    /**
     * @brief Create the DAG associated directed tree
     * 
     * @return DT DAG associeated directed tree
     */
    DT toDT();

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
     * @return true if the evaluated node is a root
     * @return false otherwise
     */
    bool isRoot(unsigned long i) { return npsum_[i] == npsum_[i + 1]; }
    /**
     * @brief Perform a depth-first search visit for the current DAG.
     * 
     * @param preorder 
     * @param postorder 
     * @param innerRank 
     * @param outerRank 
     */
    void DFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder, vector<unsigned long> &innerRank, vector<unsigned long> &outerRank);
    /**
     * @brief Perform a multithreaded depth-first search visit for the current DAG.
     * 
     * @param preorder 
     * @param postorder 
     */
    void parallelDFS(vector<unsigned long> &preorder, vector<unsigned long> &postorder);
    /**
     * @brief Assign a label to each vertex.
     * 
     * @param outerRank 
     * @param innerRank 
     */
    void labeling(vector<unsigned long> &outerRank, vector<unsigned long> &innerRank);
};

#endif // _DAG_HPP