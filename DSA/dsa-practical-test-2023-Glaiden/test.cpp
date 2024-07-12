/** 
 * @file test.cpp
 * @brief Test file for the project. 
 *        Most of the code here are derived from your DSA-AVL assignment.
 *        (heavily amended from from Matthew Mead's CS280 AVL Assignment)
 *        NOTE that for the practical test, the actual test cases will be
 *             than what you see here.
 * @author Chek
 * @date 20 Aug 2023
 */

//#define DEBUG
//#define SHOW_COUNTS 
#define FUDGE 4

#include "AVL.h"
#include "SimpleAllocator.h"
#include "prng.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <typeinfo>
#include <sstream>
#include <cstring>

using std::cout;
using std::endl;
using std::stringstream;

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
 * @param size number of ints to generate
 * @param arr array to return the ints
 * @param randomInts whether to generate random ints (note that the ints will be unique)
 */
void generateShuffledInts(int size, int* arr, bool randomInts = false) {
    // generate size number of consecutive ints
    for (int i = 0; i < size; ++i) {
        if (randomInts) {
            // generate a random int between 0 and size * 2
            auto num = Utils::randInt(0, size * 2);

            // check if i already exists in the array
            // - if it does, then generate a new random int
            while (std::find(arr, arr + i, num) != arr + i)
                num = Utils::randInt(0, size * 2);

            arr[i] = num;
        }
        else
            arr[i] = i;
    }

    // shuffle the data randomly
    Utils::srand(8, 3); // 2 seeds for the random number generator
    for (int i = 0; i < size; ++i) {
        // generate a random index
        int j = Utils::randInt(0, size - 1);

        // swap data[i] and data[j]
        swapVals(arr[i], arr[j]);
    }
}

/**
 * @brief Set the positions of the nodes in the tree printout
 * @tparam T type of AVL
 * @param tree root of the AVL
 * @param depth depth of the node
 * @param nodePositions map of node positions
 */
static int Position; // global variable to keep track of position for printing 
template <typename T>
void setTreePositions(const typename AVL<T>::BinTreeNode* tree, int depth,
                      std::map<const typename AVL<T>::BinTreeNode*,
                               std::pair<int, int>>& nodePositions) {
    if (!tree)
        return;

    // recursively set the positions of the nodes in the left subtree
    setTreePositions<T>(tree->left, depth + 1, nodePositions);

    // set the position of the current node and increment the global position
    std::pair<int, int> XY(Position++, depth);

    // add the node and its position into the map
    std::pair<const typename AVL<T>::BinTreeNode*, std::pair<int, int>> pr(tree,
                                                                           XY);
    nodePositions.insert(pr);

    // recursively set the positions of the nodes in the right subtree
    setTreePositions<T>(tree->right, depth + 1, nodePositions);
}

/**
 * @brief Set the positions of the nodes in the tree printout (overloaded)
 * @tparam T type of AVL
 * @param tree root of the AVL
 * @param nodePositions map of node positions
 */
template <typename T>
void setTreePositions(const AVL<T>& tree,
                      std::map<const typename AVL<T>::BinTreeNode*,
                               std::pair<int, int>>& nodePositions) {

    // reset the global position
    Position = 0;

    // clear the map
    nodePositions.clear();

    // start the recursion at depth 0
    setTreePositions<T>(tree.root(), 0, nodePositions);
}

/**
 * @brief Get the nodes at a given level
 * @tparam T type of AVL
 * @param level level of the nodes
 * @param nodePositions map of node positions
 * @return vector of nodes at the given level
 */
template <typename T>
std::vector<std::pair<const typename AVL<T>::BinTreeNode*, int>>
getNodesAtLevel(int level, std::map<const typename AVL<T>::BinTreeNode*,
                                    std::pair<int, int>>& nodePositions) {
    // create a vector of nodes
    // - each node is a pair of a node and its position
    std::vector<std::pair<const typename AVL<T>::BinTreeNode*, int>> nodes;

    // iterate through the map of node positions
    typename std::map<const typename AVL<T>::BinTreeNode*,
                      std::pair<int, int>>::iterator it;
    for (it = nodePositions.begin(); it != nodePositions.end(); ++it) {

        // if the node is at the given level, add it to the vector
        std::pair<int, int> XY = it->second;
        if (XY.second == level) {
            std::pair<const typename AVL<T>::BinTreeNode*, int> pr(it->first,
                                                                   XY.first);
            nodes.push_back(pr);
        }
    }

    // return the vector of nodes
    return nodes;
}

/**
 * @brief Function object to compare nodes
 * @tparam T type of AVL
 */
template <typename T> class FuncCompareNodes {
  public:
    /**
     * @brief Method to compare nodes
     * @param a first node
     * @param b second node
     * @return true if a is less than b, false otherwise
     */
    bool
    operator()(const std::pair<const typename AVL<T>::BinTreeNode*, int>& a,
               const std::pair<const typename AVL<T>::BinTreeNode*, int>& b) {
        if (a.second < b.second)
            return true;
        else
            return false;
    }
};

/**
 * Print the AVL contents in an ascii tree format
 * - first show the height and size
 * - then show the contents of the AVL
 * - show a msg if the tree is empty
 * @param avl AVL to print stats
 * @param showCounts whether to show the counts of each node
 */
template <typename T> void printAVL(const AVL<T>& avl, bool showCounts = false) {
    // if avl is empty, then print a msg and return
    if (avl.empty()) {
        cout << "  <EMPTY TREE>" << endl;
        return;
    }

    // map of the nodes and their positions
    std::map<const typename AVL<T>::BinTreeNode*, std::pair<int, int>>
        NodePositions;

    // set the positions of the nodes in the tree
    setTreePositions(avl, NodePositions);

    // print the nodes in the tree
    int height = avl.height();
    int offset = 0;
    for (int i = 0; i <= height; i++) {
        // get the nodes at the current level
        std::vector<std::pair<const typename AVL<T>::BinTreeNode*, int>> nodes =
            getNodesAtLevel<T>(i, NodePositions);

        // sort the nodes by position
        std::sort(nodes.begin(), nodes.end(), FuncCompareNodes<T>());

        // print the nodes at the current level
        typename std::vector<
            std::pair<const typename AVL<T>::BinTreeNode*, int>>::iterator iter;
        char buffer[1024 * 2] = {0}; // 2K buffer should be enough
        std::memset(buffer, ' ', 1024 * 2); // fill with spaces
        for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
            // get the data from the node
            T value = (*iter).first->data;

            // print the data
            std::stringstream ss;

#ifdef SHOW_COUNTS
            ss << value << "[" << (*iter).first->count << "]" << "{" << (*iter).first->balanceFactor << "}";
#else
            ss << value;
#endif

            // calculate the offset
            // - the offset is based on the position of the node
            // - the fudge factor determines how much space to leave between nodes
            //   (increase fudge factor if you have large numbers in the tree)
            offset = (height / 2) + iter->second * FUDGE;

            // copy the data into the buffer
            strncpy(buffer + offset, ss.str().c_str(), ss.str().length());
        }

        // print the buffer
        buffer[offset + FUDGE * 2] = 0;
        std::cout << buffer << std::endl << std::endl;
    }
}

/**
 * Print some AVL overall stats
 * @param avl AVL to print stats
 */
template <typename T> void printStats(const AVL<T>& tree) {
    // get the type of AVL
    std::string type = std::strstr(typeid(tree).name(), "AVL") ? "AVL" : "BST";

    // print the stats
    cout << "type: " << type << ", height: " << tree.height()
         << ", size: " << tree.size() << endl;
}

/**
 * @brief Create an AVL
 * @tparam T type of AVL
 * @return AVL
 */
template <typename T> AVL<T> createAVL() {

#ifdef DEBUG
    cout << "Running createAVL..." << endl;
    cout << endl;
#endif

    // create a AVL
    AVL<T> avl;

#ifdef DEBUG
    cout << "AVL after creation:" << endl << endl;
    printStats(avl);
    printAVL(avl);
    cout << endl;
#endif

    // return the AVL
    return avl;
}

/**
 * @brief Add a number of ints into a AVL
 * @param avl AVL to add ints into
 * @param size number of ints to add
 * @param sorted whether to add the data in sorted order
 * @param noPrint whether to print the AVL
 * @param randomInts whether to generate random ints
 */
template <typename T> 
void addInts(AVL<T>& avl, int size, bool sorted = false, bool noPrint = false, bool randomInts = false) {
    try {
        // print a title of the test
        cout << "Running addInts";
        if (sorted)
            cout << "(sorted)..." << endl;
        else
            cout << "..." << endl;
        cout << endl;

        // generate size number of ints
        int data[size];

        if (sorted) {
            // generate sorted ints
            for (int i = 0; i < size; ++i) {
                data[i] = i;
            }
        } else {
            // generate shuffled ints
            if (randomInts)
                generateShuffledInts(size, data, true);
            else
                generateShuffledInts(size, data);
        }

        // Add the data into the AVL
        for (int i = 0; i < size; ++i) {
            // IF DEBUG then print i and the contents
#ifdef DEBUG
            cout << "Adding " << data[i] << "..." << endl;
            cout << endl;
#endif

            avl.add(data[i]);

#ifdef DEBUG
            printAVL(avl);
            cout << endl;
#endif
        }

        // print the AVL
        cout << "AVL after adding " << size << " elements:" << endl << endl;
        printStats(avl);
        if (!noPrint)
            printAVL(avl);

    }
    catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
}

/**
 * @brief Remove a number of ints from a AVL
 * @param avl AVL to remove ints from
 * @param useClear whether to use clear() to remove the data
 * @param size number of ints to remove
 * @param sorted whether to remove the data in sorted order
 * @param noPrint whether to print the AVL
 */
template <typename T>
void removeInts(AVL<T>& avl, bool useClear = false, int size = 0, bool sorted = false, bool noPrint = false) {
    try {
        // print a title of the test
        cout << "Running removeInts";
        if (useClear)
            cout << "(using clear)..." << endl;
        else
            cout << "..." << endl;
        cout << endl;

        if (useClear) {
            // clear the AVL if we are using clear()
            avl.clear();

            // print the AVL
            cout << "AVL after clearing:" << endl << endl;
            printStats(avl);
            printAVL(avl);
        } else {
            // create an array of ints to remove so that we do not repeat
            // - generate size number of ints, either sorted or shuffled
            int totalVals = avl.size();
            int valsToRemove[totalVals];

            if (sorted) {
                // generate sorted ints
                for (int i = 0; i < totalVals; ++i) {
                    valsToRemove[i] = i;
                }
            } else {
                // generate shuffled ints
                generateShuffledInts(totalVals, valsToRemove);
            }

            // remove size number of data from the AVL
            for (int i = 0; i < size; ++i) {
                // get a random int between 0 and size - 1 without repeating
                // - loop back if we reach the end of the array
                int valToRemove = valsToRemove[i % totalVals];

                // IF DEBUG then print i and the contents
#ifdef DEBUG
                cout << "Removing " << valToRemove << "..." << endl;
                cout << endl;
#endif

                avl.remove(valToRemove);

#ifdef DEBUG
                printAVL(avl);
                cout << endl;
#endif
            }

            // print the AVL
            cout << "AVL after removing " << size << " elements:" << endl;
            printStats(avl);
            if (!noPrint)
                printAVL(avl);
        }
    }
    catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
}

/**
 * @brief Find an int in a AVL
 *        - need to print the number of compares
 *        - need to print the result of the find
 * @param avl AVL to find the int in
 * @param val int to find
 */
template <typename T> 
void findInt(AVL<T>& avl, int val) {
    try {
        // print a title of the test
        cout << "Running findInt..." << endl;
        cout << endl;

        // find the int in the AVL
        unsigned compares = 0;
        bool found = avl.find(val, compares);

        // print the result
        cout << "  Value " << val << " is ";
        if (found)
            cout << "FOUND ";
        else
            cout << "NOT FOUND ";

        // print the number of compares concatenated to be previous line
        cout << "after " << compares << " compares" << endl;
    }
    catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
    cout << endl;
}

/**
 * The main function that configure and run all the test cases.
 * NOTE that in the practical test, the actual test cases will be
 *      more than what you see here.
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 */
int main(int argc, char* argv[]) {
    // test number
    int test = 0;

    // check for command line arguments
    if (argc > 1)
        test = atoi(argv[1]);

    // same avl inits across all tests
    AVL<int> avl = createAVL<int>();

    // stringstream for inorder traversal
    stringstream inorderSS;

    // run the test
    switch (test) {
    case 0:
        cout << "=== Test whether we can add one single int into a tree ===" << endl << endl;
        addInts<int>(avl, 1);
        break;
    case 1:
        cout << "=== Test a trivial number of int adds and removes into a tree ===" << endl << endl;
        addInts<int>(avl, 3);
        removeInts<int>(avl, false, 1);
        break;
    case 2:
        cout << "=== Test a small number of random int adds into an AVL tree ===" << endl << endl;
        addInts<int>(avl, 5);
        break;
    case 3:
        cout << "=== Test a large number of sorted int adds into an AVL tree ===" << endl << endl;
        addInts<int>(avl, 20, true);
        break;
    case 4:
        cout << "=== Test a small number of random int adds and removes into an AVL tree ===" << endl;
        addInts<int>(avl, 8);
        removeInts<int>(avl, false, 2, true);
        break;
    case 5:
        cout << "=== Test a large number of sorted int adds and removes into an AVL tree ===" << endl;
        addInts<int>(avl, 20, true);
        removeInts<int>(avl, false, 10, true);
        break;
    case 6:
        cout << "=== Test the inorder traversal of the AVL tree ===" << endl;
        addInts<int>(avl, 20, false, false, true);
        inorderSS = avl.printInorder();
        cout << "Inorder traversal: " << inorderSS.str() << endl;
        break;
    default:
        cout << "Please select a valid test." << endl;
        break;
    }

    cout << "========================================" << endl;

    return 0;
}
