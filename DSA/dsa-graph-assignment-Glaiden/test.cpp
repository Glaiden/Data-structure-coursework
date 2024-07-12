/** 
 * @file test.cpp
 * @brief Test file for the project. Adapted from Matthew Mead's Graph Assignment
 *        in DigiPen's CS280 class.
 * @author Chek
 * @date 20 Aug 2023
 */

//#define DEBUG

#include "ALGraph.h"
#include "prng.h" // for Utils::srand and Utils::randInt
#include <iomanip> // for setw
#include <iostream> // for std::cout
#include <vector> // for std::vector
#include <algorithm> // for std::sort
#include <cstdlib> // for atoi
#include <ctime> // for time
#include <typeinfo> // for typeid
#include <sstream> // for std::stringstream
#include <cstring> // for std::strcmp
#include <fstream> // for std::ofstream

using std::cout;
using std::endl;
using std::setw;

/**
 * @brief Helper function to swap two values
 * @tparam T type of values to swap
 * @param a first value
 * @param b second value
 */
template <typename T> void swapVals(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

/**
 * @brief helper function to generate a number of shuffled consecutive ints
 *        - NOTE that here the ints are 1..n, not 0..n-1
 * @param size number of ints to generate
 * @param arr array to return the ints
 */
void generateShuffledInts(int size, int* arr) {
    // generate size number of consecutive ints
    for (int i = 0; i < size; ++i) {
        arr[i] = i+1; // 1..n
    }

    // shuffle the data randomly
    Utils::srand(8, 1);
    for (int i = 0; i < size; ++i) {
        // generate a random index
        int j = Utils::randInt(0, size - 1);

        // swap data[i] and data[j]
        swapVals(arr[i], arr[j]);
    }
}

/**
 * Generate a Graphviz file for a given graph
 * @param graph the graph to generate the file for
 * @param filename the name of the file to generate
 * @param isDirected true if the graph is directed
 */
void generateGraphvizFile(const ALGraph& graph, const std::string& filename, bool isDirected) {
    cout << "Generating Graphviz file..." << endl;

    // open the file
    std::ofstream file(filename);

    // set a char* to represent the edge style
    const char* edgeStyle = isDirected ? " -> " : " -- ";

    // write the header
    // TODO: see whether "G" is needed and whether "strict" is needed
    //file << (isDirected ? "digraph" : "graph") << " G {" << endl;
    file << (isDirected ? "digraph" : "strict graph") << " {" << endl;

    // write the nodes by iterating through the adjacency list
    AdjList adjList = graph.getAdjList();
    int count = 1;
    for (auto adjInfoList : adjList) {
        for (auto adjInfo : adjInfoList) {
            file << setw(6) << count << " " << edgeStyle << " " << setw(3)
                 << adjInfo.id << " [label=\"" << adjInfo.weight << "\"];"
                 << endl;
        }
        ++count;
    }

    // write the footer
    file << "}" << endl;

    // close the file
    file.close();
}

/**
 * Dump the graph by printing out the adjacency list.
 * @param graph the graph to dump
 */
void dumpGraph(const ALGraph& graph) {
    cout << "Dumping the graph..." << endl;

    AdjList adjList = graph.getAdjList();
    int count = 1;
    for (auto adjInfoList : adjList) {
        cout << "ID: [" << setw(2) << count++ << "]";
        for (auto adjInfo : adjInfoList) {
            cout << " -- ";
            cout << setw(2) << adjInfo.weight << " --> ";
            cout << "[" << setw(2) << adjInfo.id << "]";
        }
        cout << endl;
    }
}

/**
 * @brief Dump the path nodes from Dijsktra's algorithm.
 * @param startNode the start node
 * @param dijkstraInfoList the list of DijkstraInfo
 */
void dumpDijkstraInfo(const std::vector<DijkstraInfo>& dijkstraInfoList) {
    cout << "Dumping the DijkstraInfo..." << endl;
    int count = 1;

    for (auto dijkstraInfo : dijkstraInfoList) {
        cout << "ID: [" << setw(2) << count++ << "] ";
        cout << " cost: " << setw(3) << dijkstraInfo.cost;
        cout << " path: ";
        for (auto id : dijkstraInfo.path) {
            // print >> if not startNode
            if (id != dijkstraInfo.path.front())
                cout << " >> ";
            cout << setw(2) << id;
        }
        cout << endl;
    }
    cout << endl;
}

/**
 * @brief Return a directed or undirected graph given a number of vertices and edges
 *        - all vertices are numbered 1..n
 *        - every vertex must be part of at least one edge
 *        - no self loops for the edges
 * @param numVertices number of vertices
 * @param numEdges number of edges
 * @param isDirected true if the graph is directed
 * @return a graph
 */
ALGraph getRandomGraph(int numVertices, int numEdges, bool isDirected) {
    cout << "Generating a graph with " << numVertices << " vertices and "
         << numEdges << " edges..." << endl;

    // create a graph
    ALGraph graph(numVertices);

    // create an array of shuffled ints
    int* shuffledInts = new int[numVertices];
    generateShuffledInts(numVertices, shuffledInts);

#ifdef DEBUG
    cout << "Shuffled ints: ";
    for (int i = 0; i < numVertices; ++i) {
        cout << shuffledInts[i] << " ";
    }
    cout << endl;
#endif

    // To add edges such that every vertex must be part of at least one edge,
    // we first create a cycle of edges
    for (int i = 0; i < numVertices; ++i) {
        // generate a random weight
        int weight = Utils::randInt(1, 99);

        // get the current vertex
        auto source = shuffledInts[i];

        // get the next vertex
        auto destination = shuffledInts[(i + 1) % numVertices];
        
#ifdef DEBUG
        cout << "source: " << source << " destination: " << destination
             << " weight: " << weight << endl;
#endif

        // add an edge between the two vertices
        if (isDirected) {
            graph.addDEdge(source, destination, weight);
        } else {
            graph.addUEdge(source, destination, weight);
        }
    }

    // then we randomly add some more edges
    numEdges -= numVertices; // subtract the number of edges from the cycle
    for (int i = 0; i < numEdges; ++i) {
        // generate a random weight
        int weight = Utils::randInt(1, 99);

        // generate a random source and destination index
        // - generate a new destination index until it is different from the
        //   source index
        auto sourceIndex = Utils::randInt(0, numVertices - 1);
        auto destinationIndex = Utils::randInt(0, numVertices - 1);
        while (destinationIndex == sourceIndex) {
            destinationIndex = Utils::randInt(0, numVertices - 1);
        }

        // get the source and destination
        int source = shuffledInts[sourceIndex];
        int destination = shuffledInts[destinationIndex];

#ifdef DEBUG
        cout << "source: " << source << " destination: " << destination
             << " weight: " << weight << endl;
#endif

        // add the edge
        if (isDirected)
            graph.addDEdge(source, destination, weight);
        else
            graph.addUEdge(source, destination, weight);
    }

    // delete the array
    delete[] shuffledInts;

    return graph;
}

/**
 * @brief Return a graph based on a certain pre-defined type number.
 *        The graph will be manually created based on the type.
 *        - type 0: a tiny undirected graph with 3 vertices and 3 edges
 *        - type 1: a small undirected graph with 6 vertices and 10 edges
 *        - type 2: a medium directed graph with 16 vertices and 33 edges
 * @param type the type of graph
 * @param dump true if the graph should be dumped
 * @return a graph
 */
ALGraph getGraph(int type, bool isDump = true) {
    if (isDump)
        cout << "Generating a graph of type " << type << "..." << endl;

    // create a graph
    ALGraph graph(0);
    auto isDirected = false;

    // add edges
    switch (type) {

    case 0:
        // create a tiny undirected graph
        graph = ALGraph(3);
        isDirected = false;
        graph.addUEdge(1, 2, 10);
        graph.addUEdge(1, 3, 20);
        graph.addUEdge(2, 3, 30);
        break;

    case 1:
        // create a small undirected graph
        graph = ALGraph(6);
        isDirected = false;
        graph.addUEdge(1, 2, 8);
        graph.addUEdge(1, 3, 16);
        graph.addUEdge(1, 5, 13);
        graph.addUEdge(2, 3, 7);
        graph.addUEdge(2, 4, 17);
        graph.addUEdge(2, 5, 11);
        graph.addUEdge(2, 6, 10);
        graph.addUEdge(3, 4, 5);
        graph.addUEdge(4, 5, 14);
        graph.addUEdge(4, 6, 6);
        break;

    case 2:
        // create a medium directed graph
        graph = ALGraph(16);
        isDirected = true;

        graph.addDEdge(1, 2, 1);
        graph.addDEdge(1, 5, 3);
        graph.addDEdge(2, 3, 2);
        graph.addDEdge(2, 5, 1);
        graph.addDEdge(3, 4, 3);

        graph.addDEdge(3, 7, 5);
        graph.addDEdge(4, 8, 2);
        graph.addDEdge(5, 6, 3);
        graph.addDEdge(5, 9, 2);
        graph.addDEdge(5, 10, 1);

        graph.addDEdge(6, 2, 6);
        graph.addDEdge(6, 10, 1);
        graph.addDEdge(7, 2, 2);
        graph.addDEdge(7, 8, 1);
        graph.addDEdge(7, 6, 1);

        graph.addDEdge(7, 10, 1);
        graph.addDEdge(8, 3, 1);
        graph.addDEdge(9, 13, 4);
        graph.addDEdge(9, 14, 5);
        graph.addDEdge(10, 9, 2);

        graph.addDEdge(10, 14, 1);
        graph.addDEdge(11, 7, 3);
        graph.addDEdge(11, 10, 2);
        graph.addDEdge(11, 12, 2);
        graph.addDEdge(12, 7, 2);

        graph.addDEdge(12, 8, 3);
        graph.addDEdge(12, 16, 1);
        graph.addDEdge(14, 13, 2);
        graph.addDEdge(14, 15, 1);
        graph.addDEdge(15, 10, 5);

        graph.addDEdge(15, 11, 2);
        graph.addDEdge(16, 11, 3);
        graph.addDEdge(16, 15, 2);
        break;

    case 3:
        // create a medium directed graph with more complex weights
        graph = ALGraph(16);
        isDirected = true;

        graph.addDEdge(1, 2, 11);
        graph.addDEdge(1, 5, 13);
        graph.addDEdge(2, 3, 21);
        graph.addDEdge(2, 5, 31);
        graph.addDEdge(3, 4, 29);

        graph.addDEdge(3, 7, 15);
        graph.addDEdge(4, 8, 12);
        graph.addDEdge(5, 6, 13);
        graph.addDEdge(5, 9, 23);
        graph.addDEdge(5, 10, 29);

        graph.addDEdge(6, 2, 16);
        graph.addDEdge(6, 10, 31);
        graph.addDEdge(7, 2, 26);
        graph.addDEdge(7, 8, 10);
        graph.addDEdge(7, 6, 12);

        graph.addDEdge(7, 10, 14);
        graph.addDEdge(8, 3, 31);
        graph.addDEdge(9, 13, 45);
        graph.addDEdge(9, 14, 15);
        graph.addDEdge(10, 9, 2);

        graph.addDEdge(10, 14, 51);
        graph.addDEdge(11, 7, 33);
        graph.addDEdge(11, 10, 22);
        graph.addDEdge(11, 12, 12);
        graph.addDEdge(12, 7, 27);

        graph.addDEdge(12, 8, 13);
        graph.addDEdge(12, 16, 41);
        graph.addDEdge(14, 13, 28);
        graph.addDEdge(14, 15, 19);
        graph.addDEdge(15, 10, 35);

        graph.addDEdge(15, 11, 23);
        graph.addDEdge(16, 11, 33);
        graph.addDEdge(16, 15, 22);
        break;

    case 4:
        // create a medium undirected graph
        graph = ALGraph(10);
        isDirected = true;

        graph.addUEdge(1, 2, 33);
        graph.addUEdge(1, 3, 10);
        graph.addUEdge(1, 4, 56);
        graph.addUEdge(2, 4, 13);
        graph.addUEdge(2, 5, 21);
        graph.addUEdge(3, 4, 23);
        graph.addUEdge(3, 6, 24);
        graph.addUEdge(3, 7, 65);
        graph.addUEdge(4, 5, 51);
        graph.addUEdge(4, 7, 20);
        graph.addUEdge(5, 7, 17);
        graph.addUEdge(5, 8, 35);
        graph.addUEdge(6, 7, 40);
        graph.addUEdge(6, 9, 72);
        graph.addUEdge(7, 8, 99);
        graph.addUEdge(7, 9, 45);
        graph.addUEdge(7, 10, 42);
        graph.addUEdge(8, 10, 38);
        graph.addUEdge(9, 10, 83);
        break;

    case 5:
        // create a large undirected graph
        graph = getRandomGraph(99, 388, true);
        break;

    default:
        cout << "Invalid graph type " << type << endl;
        break;
    }


    if (isDump) {
        // dump the graph
        dumpGraph(graph);

        // generate graphviz with filename including the type
        std::stringstream ss;
        ss << "graph" << type << ".dot";
        generateGraphvizFile(graph, ss.str(), isDirected);
    }
    
    return graph;
}

/**
 * @brief Test Dijkstra's algorithm on a graph
 * @param graph the graph to test
 * @param startNode the start node
 */
void testDijkstra(const ALGraph& graph, unsigned startNode) {
    cout << "Testing Dijkstra's algorithm..." << endl;

    // run Dijkstra's algorithm
    auto dijkstraInfoList = graph.dijkstra(startNode);

    // dump the DijkstraInfo
    dumpDijkstraInfo(dijkstraInfoList);
}

/**
 * The main function
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 */
int main(int argc, char* argv[]) {
    // test number
    int test = 0;

    // check for command line arguments
    if (argc > 1)
        test = atoi(argv[1]);

    // reuse graph var across all cases
    auto graph = getGraph(0, false);

    // run the test
    switch (test) {
    case 0:
        cout << "=== Test creating a tiny undirected graph ===" << endl << endl;
        graph = getGraph(0);
        break;
    case 1:
        cout << "=== Test creating a small undirected graph ===" << endl << endl;
        graph = getGraph(1);
        break;
    case 2:
        cout << "=== Test creating a medium directed graph ===" << endl << endl;
        graph = getGraph(2);
        break;
    case 3:
        cout << "=== Test creating a medium undirected graph with more complex weights ===" << endl << endl;
        graph = getGraph(3);
        break;
    case 4:
        cout << "=== Test creating a medium undirected graph ===" << endl << endl;
        graph = getGraph(4);
        break;
    case 5:
        cout << "=== Test creating a large undirected graph ===" << endl << endl;
        graph = getGraph(5);
        break;
    case 6:
        cout << "=== Test Dijkstra's on the small undirected graph in case 1 ===" << endl << endl;
        graph = getGraph(1, false);
        testDijkstra(graph, 1);
        break;
    case 7:
        cout << "=== Test Dijkstra's on the medium directed graph in case 2 ===" << endl << endl;
        graph = getGraph(2, false);
        testDijkstra(graph, 1);
        break;
    case 8:
        cout << "=== Test Dijkstra's on the medium undirected graph in case 3 ===" << endl << endl;
        graph = getGraph(3, false);
        testDijkstra(graph, 1);
        break;
    case 9:
        cout << "=== Test Dijkstra's on the medium undirected graph in case 4 ===" << endl << endl;
        graph = getGraph(4, false);
        testDijkstra(graph, 1);
        break;
    case 10:
        cout << "=== Test Dijkstra's on the large undirected graph in case 5 ===" << endl << endl;
        graph = getGraph(5, false);
        testDijkstra(graph, 1);
        break;
    default:
        cout << "Please select a valid test." << endl;
        break;
    }

    cout << "========================================" << endl;

    return 0;
}
