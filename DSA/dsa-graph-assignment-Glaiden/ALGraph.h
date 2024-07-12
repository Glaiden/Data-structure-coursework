/**
 * @file ALGraph.h
 * @author Chek
 * @brief ALGraph class definition
 *        Note that this file is lengthy
 *        as it tries to contain as much information as possible
 *        for students to understand the requirements
 * @date 9 Oct 2023
 * TODO: consider using SimpleAllocator
 */
#ifndef ALGRAPH_H
#define ALGRAPH_H

// include any other libraries you need
#include <vector>

/**
 * @brief AdjacencyInfo struct
 *        This struct is used to store the adjacency information of a vertex
 *        It contains the vertex id and the weight of the edge
 *        This will be the type of the elements in the adjacency list
 */
struct AdjInfo {
    unsigned id; // 1..n
    unsigned weight;

    /**
     * @brief operator< overload
     * @param rhs the right hand side of the operator
     * @return true if the weight of this is less than the weight of rhs
     */
    bool operator<(const AdjInfo& rhs) const { return weight < rhs.weight; }

    /**
     * @brief operator> overload
     * @param rhs the right hand side of the operator
     * @return true if the weight of this is greater than the weight of rhs
     */
    bool operator>(const AdjInfo& rhs) const { return weight > rhs.weight; }
};

// Define AdjList as a vector of vectors of AdjInfo
// - the main vector's element index is the vertex id of the source vertex.
// - each element in the vector is another vector of AdjInfo, 
//   which contains the destination vertex id and the weight.
using AdjList = std::vector<std::vector<AdjInfo>>;

/**
 * @brief DijkstraInfo struct
 *        This struct is used to return the result of Dijkstra's algorithm.
 *        It contains the ordered vertex ids of the resultant **path** 
 *        and the total **cost** of the path.
 */
struct DijkstraInfo {
    unsigned cost;
    std::vector<unsigned> path;
};

/**
 * @brief ALGraph class
 *        This class represents a graph using the adjacency list concept.
 */
class ALGraph {
public:

    /**
     * @brief Constructor
     * @param size the number of vertices in the graph
     */
    ALGraph(unsigned size);

    /**
     * @brief Destructor
     */
    ~ALGraph();

    /**
     * @brief Add a directed edge to the graph
     * @param source the source vertex id
     * @param destination the destination vertex id
     * @param weight the weight of the edge
     */
    void addDEdge(unsigned source, unsigned destination, unsigned weight);

    /**
     * @brief Add an undirected edge to the graph
     *        This is done by adding two directed edges using AddDEdge
     * @param node1 one of the vertex id
     * @param node2 the other vertex id
     * @param weight the weight of the edge
     */
    void addUEdge(unsigned node1, unsigned node2, unsigned weight);

    /**
     * @brief Dijkstra's algorithm
     *        This method finds the shortest path from the start node to all other nodes.
     *        It returns a vector of DijkstraInfo, one for each node.
     * @param start_node the start node id
     * @return a vector of DijkstraInfo, one for each node
     */
    std::vector<DijkstraInfo> dijkstra(unsigned start) const;

    /**
     * @brief Get the adjacency list
     * @return the adjacency list
     */
    AdjList getAdjList() const;

private:
    // Example of other private fields and methods.
    // Feel free to change/add any private fields/methods/structs/classes
    unsigned size_;
    AdjList adjList_;
};

#endif
