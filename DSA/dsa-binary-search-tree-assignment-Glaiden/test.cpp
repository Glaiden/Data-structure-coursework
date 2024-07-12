/** @file test.cpp
 * @brief Test file for the project. Adapted from Matthew Mead's AVL Assignment
 *        in DigiPen's CS280 class.
 * @author Chek
 * @date 20 Aug 2023
 */

//#define DEBUG
//#define SHOW_COUNTS 
#define FUDGE 4

#include "BST.h"
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
 */
void generateShuffledInts(int size, int* arr) {
    // generate size number of consecutive ints
    for (int i = 0; i < size; ++i) {
        arr[i] = i;
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
 * @brief helper function to generate a number of shuffled consecutive chars
 * @param size number of chars to generate
 * @param arr array to return the chars
 */
void generateShuffledChars(int size, char* arr) {
    // generate size number of consecutive chars
    for (int i = 0; i < size; ++i) {
        arr[i] = 'a' + i;
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
 * @brief Set the positions of the nodes in the tree printout
 * @tparam T type of BST
 * @param tree root of the BST
 * @param depth depth of the node
 * @param nodePositions map of node positions
 */
static int Position; // global variable to keep track of position for printing 
template <typename T>
void setTreePositions(const typename BST<T>::BinTreeNode* tree, int depth,
                      std::map<const typename BST<T>::BinTreeNode*,
                               std::pair<int, int>>& nodePositions) {
    if (!tree)
        return;

    // recursively set the positions of the nodes in the left subtree
    setTreePositions<T>(tree->left, depth + 1, nodePositions);

    // set the position of the current node and increment the global position
    std::pair<int, int> XY(Position++, depth);

    // add the node and its position into the map
    std::pair<const typename BST<T>::BinTreeNode*, std::pair<int, int>> pr(tree,
                                                                           XY);
    nodePositions.insert(pr);

    // recursively set the positions of the nodes in the right subtree
    setTreePositions<T>(tree->right, depth + 1, nodePositions);
}

/**
 * @brief Set the positions of the nodes in the tree printout (overloaded)
 * @tparam T type of BST
 * @param tree root of the BST
 * @param nodePositions map of node positions
 */
template <typename T>
void setTreePositions(const BST<T>& tree,
                      std::map<const typename BST<T>::BinTreeNode*,
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
 * @tparam T type of BST
 * @param level level of the nodes
 * @param nodePositions map of node positions
 * @return vector of nodes at the given level
 */
template <typename T>
std::vector<std::pair<const typename BST<T>::BinTreeNode*, int>>
getNodesAtLevel(int level, std::map<const typename BST<T>::BinTreeNode*,
                                    std::pair<int, int>>& nodePositions) {
    // create a vector of nodes
    // - each node is a pair of a node and its position
    std::vector<std::pair<const typename BST<T>::BinTreeNode*, int>> nodes;

    // iterate through the map of node positions
    typename std::map<const typename BST<T>::BinTreeNode*,
                      std::pair<int, int>>::iterator it;
    for (it = nodePositions.begin(); it != nodePositions.end(); ++it) {

        // if the node is at the given level, add it to the vector
        std::pair<int, int> XY = it->second;
        if (XY.second == level) {
            std::pair<const typename BST<T>::BinTreeNode*, int> pr(it->first,
                                                                   XY.first);
            nodes.push_back(pr);
        }
    }

    // return the vector of nodes
    return nodes;
}

/**
 * @brief Function object to compare nodes
 * @tparam T type of BST
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
    operator()(const std::pair<const typename BST<T>::BinTreeNode*, int>& a,
               const std::pair<const typename BST<T>::BinTreeNode*, int>& b) {
        if (a.second < b.second)
            return true;
        else
            return false;
    }
};

/**
 * Print the BST contents in an ascii tree format
 * - first show the height and size
 * - then show the contents of the BST
 * - show a msg if the tree is empty
 * @param bst BST to print stats
 * @param showCounts whether to show the counts of each node
 */
template <typename T> void printBST(const BST<T>& bst, bool showCounts = false) {
    // if bst is empty, then print a msg and return
    if (bst.empty()) {
        cout << "  <EMPTY TREE>" << endl;
        return;
    }

    // map of the nodes and their positions
    std::map<const typename BST<T>::BinTreeNode*, std::pair<int, int>>
        NodePositions;

    // set the positions of the nodes in the tree
    setTreePositions(bst, NodePositions);

    // print the nodes in the tree
    int height = bst.height();
    int offset = 0;
    for (int i = 0; i <= height; i++) {
        // get the nodes at the current level
        std::vector<std::pair<const typename BST<T>::BinTreeNode*, int>> nodes =
            getNodesAtLevel<T>(i, NodePositions);

        // sort the nodes by position
        std::sort(nodes.begin(), nodes.end(), FuncCompareNodes<T>());

        // print the nodes at the current level
        typename std::vector<
            std::pair<const typename BST<T>::BinTreeNode*, int>>::iterator iter;
        char buffer[1024 * 2] = {0}; // 2K buffer should be enough
        std::memset(buffer, ' ', 1024 * 2); // fill with spaces
        for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
            // get the data from the node
            T value = (*iter).first->data;

            // print the data
            std::stringstream ss;

#ifdef SHOW_COUNTS
            ss << value << "[" << (*iter).first->count << "]";
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
 * Print some BST overall stats
 * @param bst BST to print stats
 */
template <typename T> void printBSTStats(const BST<T>& bst) {
    // get the type of BST
    std::string bstType = std::strstr(typeid(bst).name(), "BST") ? "BST" : "AVL";

    // print the stats
    cout << "type: " << bstType << ", height: " << bst.height()
         << ", size: " << bst.size() << endl;
}

/**
 * Create a BST given a T.
 * - the BST is created with the default allocator
 * @tparam T type of BST
 * @return BST
 */
template <typename T> BST<T> createBST() {
    // print a title of the test
#ifdef DEBUG
    cout << "Running createBST..." << endl;
    cout << endl;
#endif

    // create a BST
    BST<T> bst;

#ifdef DEBUG
    // print the BST
    cout << "BST after creation:" << endl << endl;
    printBSTStats(bst);
    printBST(bst);
    cout << endl;
#endif

    // return the BST
    return bst;
}

/**
 * @brief Add a number of random chars into a BST
 *       - need to detect the BSTExceptions
 * @param bst BST to add chars into
 * @param size number of chars to add
 * @param noPrint whether to print the BST
 */
template <typename T>
void addChars(BST<T>& bst, int size, bool noPrint = false) {
    try {
        // print a title of the test
        cout << "Running addChars..." << endl;
        cout << endl;

        // generate size number of shuffled chars
        char data[size];
        generateShuffledChars(size, data);

        // Add the data into the BST
        for (int i = 0; i < size; ++i) {
            // IF DEBUG then print i and the contents
#ifdef DEBUG
            cout << "Adding " << data[i] << "..." << endl;
            cout << endl;
#endif

            bst.add(data[i]);

#ifdef DEBUG
            printBST(bst);
            cout << endl;
#endif
        }

        // print the BST
        cout << "BST after adding " << size << " elements:" << endl << endl;
        printBSTStats(bst);
        if (!noPrint)
            printBST(bst);

    }

    catch (BSTException& e) {
        // print exception message
        cout << "  !!! BSTException: " << e.what() << endl;
    } catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
}

/**
 * @brief Add a number of ints into a BST
 *        - need to detect the BSTExceptions
 * @param bst BST to add ints into
 * @param size number of ints to add
 * @param sorted whether to add the data in sorted order
 * @param noPrint whether to print the BST
 */
template <typename T>
void addInts(BST<T>& bst, int size, bool sorted = false, bool noPrint = false) {
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
            generateShuffledInts(size, data);
        }

        // Add the data into the BST
        for (int i = 0; i < size; ++i) {
            // IF DEBUG then print i and the contents
#ifdef DEBUG
            cout << "Adding " << data[i] << "..." << endl;
            cout << endl;
#endif

            bst.add(data[i]);

#ifdef DEBUG
            printBST(bst);
            cout << endl;
#endif
        }

        // print the BST
        cout << "BST after adding " << size << " elements:" << endl << endl;
        printBSTStats(bst);
        if (!noPrint)
            printBST(bst);
    }

    catch (BSTException& e) {
        // print exception message
        cout << "  !!! BSTException: " << e.what() << endl;
    } catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
}

/**
 * @brief Remove a number of ints from a BST
 *        - need to detect the BSTExceptions
 * @param bst BST to remove ints from
 * @param useClear whether to use clear() to remove the data
 * @param size number of ints to remove
 * @param noPrint whether to print the BST
 */
template <typename T>
void removeInts(BST<T>& bst, bool useClear = false, int size = 0, bool noPrint = false) {
    try {
        // print a title of the test
        cout << "Running removeInts";
        if (useClear)
            cout << "(using clear)..." << endl;
        else
            cout << "..." << endl;
        cout << endl;

        if (useClear) {
            // clear the BST if we are using clear()
            bst.clear();

            // print the BST
            cout << "BST after clearing:" << endl << endl;
            printBSTStats(bst);
            printBST(bst);
        } else {
            // create an array of ints to remove so that we do not repeat
            // - use the shuffled data array to randomize the order of removal
            int totalVals = bst.size();
            int valsToRemove[totalVals];
            generateShuffledInts(totalVals, valsToRemove);

            // remove size number of data from the BST
            for (int i = 0; i < size; ++i) {
                // get a random int between 0 and size - 1 without repeating
                // - loop back if we reach the end of the array
                int valToRemove = valsToRemove[i % totalVals];

                // IF DEBUG then print i and the contents
#ifdef DEBUG
                cout << "Removing " << valToRemove << "..." << endl;
                cout << endl;
#endif

                bst.remove(valToRemove);

#ifdef DEBUG
                printBST(bst);
                cout << endl;
#endif
            }

            // print the BST
            cout << "BST after removing " << size << " elements:" << endl;
            printBSTStats(bst);
            if (!noPrint)
                printBST(bst);
        }
    }
    catch (BSTException& e) {
        // print exception message
        cout << "  !!! BSTException: " << e.what() << endl;
    } catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
}

/**
 * @brief Find an int in a BST
 *        - need to detect the BSTExceptions
 *        - need to print the number of compares
 *        - need to print the result of the find
 * @param bst BST to find the int in
 * @param val int to find
 */
template <typename T> 
void findInt(BST<T>& bst, int val) {
    try {
        // print a title of the test
        cout << "Running findInt..." << endl;
        cout << endl;

        // find the int in the BST
        unsigned compares = 0;
        bool found = bst.find(val, compares);

        // print the result
        cout << "  Value " << val << " is ";
        if (found)
            cout << "FOUND ";
        else
            cout << "NOT FOUND ";

        // print the number of compares concatenated to be previous line
        cout << "after " << compares << " compares" << endl;
    }
    catch (BSTException& e) {
        // print exception message
        cout << "  !!! BSTException: " << e.what() << endl;
    } catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
    cout << endl;
}

/**
 * @brief Test the cpy ctor or assignment operator
 * @param bst BST to test
 * @param isAssignment a toggle to test the assignment operator
 */
template <typename T> void testCopy(BST<T>& bst, bool isAssignment) {
    // print a title of the test
    cout << "Running testCopy";
    if (isAssignment)
        cout << " (assignment)..." << endl;
    else
        cout << " (copy ctor)..." << endl;
    cout << endl;

    // add different ints into BST1 and BST2
    cout << "Adding ints into BST1" << endl << endl;
    addInts<int>(bst, 5);
    cout << "BST1:" << endl << endl;
    printBSTStats(bst);
    printBST(bst);
    cout << endl;

    if (isAssignment) {
        // create a new BST
        cout << "Creating another BST(2)..." << endl << endl;
        BST<T> bst2 = createBST<T>();

        cout << "And now adding different number of ints into BST2..." << endl
             << endl;
        addInts<int>(bst2, 10);
        cout << "BST2:" << endl << endl;
        printBSTStats(bst2);
        printBST(bst2);
        cout << endl;

        cout << "Assigning BST1 to BST2..." << endl << endl;
        bst2 = bst;
        cout << "Assigned BST2:" << endl << endl;
        printBSTStats(bst2);
        printBST(bst2);
        cout << endl;
    } else {
        // copy bst to bst2
        cout << "Constructing a new BST2 with BST1 as input..." << endl << endl;
        BST<T> bst2(bst);
        cout << "Copy constructed BST2:" << endl << endl;
        printBSTStats(bst2);
        printBST(bst2);
        cout << endl;
    }
}

/**
 * @brief Use subscript operator to get a value in a BST
 *       - need to detect the BSTExceptions
 *       - need to print the result of the subscript operator
 *       - need to print the result of the find
 * @param bst BST to test
 * @param index index of element to retrieve
 */
template <typename T> void testSubscript(BST<T>& bst, int index) {
    try {
        // print a title of the test
        cout << "Running testSubscript..." << endl;
        cout << endl;

        // get the value at the index
        T value = bst[index]->data;

        // print the result
        cout << "  Value at index " << index << " is " << value << endl;
    }
    catch (BSTException& e) {
        // print exception message
        cout << "  !!! BSTException: " << e.what() << endl;
    } catch (std::exception& e) {
        // print exception message
        cout << "  !!! std::exception: " << e.what() << endl;
    } catch (...) {
        // print exception message
        cout << "  !!! Unknown exception" << endl;
    }
    cout << endl;
}

/**
 * The main function
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * TODO alignmentTest next time. Yes we do not have a test for alignment
 *      so students do not really need to implement alignment in order
 *      to pass the current tests. But it is still a recommended feature.
 *      Students are also encouraged to write their own tests.
 */
int main(int argc, char* argv[]) {
    // test number
    int test = 0;

    // check for command line arguments
    if (argc > 1)
        test = atoi(argv[1]);

    // create var to hold allocator
    SimpleAllocator* allocator = nullptr;

    // same bst inits across all tests
    BST<int> bst = createBST<int>();
    BST<char> charBST = createBST<char>();

    // run the test
    switch (test) {
    case 0:
        cout << "=== Test a small number of random int adds into a BST ===" << endl << endl;
        addInts<int>(bst, 5);
        break;
    case 1:
        cout << "=== Test a larger number of random int adds into a BST ===" << endl;
        addInts<int>(bst, 20);
        break;
    case 2:
        cout << "=== Test a larger number of sorted adds into a BST ===" << endl;
        addInts<int>(bst, 20, true);
        break;
    case 3:
        cout << "=== Test some random adds and removes on a BST ===" << endl;
        addInts<int>(bst, 8);
        removeInts<int>(bst, false, 2);
        break;
    case 4:
        cout << "=== Test clear(ing) all on a BST ===" << endl;
        addInts<int>(bst, 8);
        removeInts<int>(bst, true);
        break;
    case 5:
        cout << "=== Test removing too many on a BST ===" << endl;
        addInts<int>(bst, 5);
        removeInts<int>(bst, false, 7);
        break;
    case 6:
        cout << "=== Test find(ing) a few things in a BST ===" << endl;
        addInts<int>(bst, 9);
        findInt<int>(bst, 5);
        findInt<int>(bst, 8);
        findInt<int>(bst, 7);
        findInt<int>(bst, 9);
        findInt<int>(bst, -1);
        break;
    case 7:
        cout << "=== Test BST assignment operator ===" << endl;
        testCopy<int>(bst, true);
        break;
    case 8:
        cout << "=== Test BST copy operator ===" << endl;
        testCopy<int>(bst, false);
        break;
    case 9:
        cout << "=== Test BST subscript operator ===" << endl;
        addChars<char>(charBST, 10);
        testSubscript(charBST, 5);
        testSubscript(charBST, 0);
        testSubscript(charBST, 9);
        testSubscript(charBST, 10);
        break;
    case 10:
        cout << "=== Stress Test BST ===" << endl;
        //clock_t start, timeTaken;
        //start = clock();
        addInts<int>(bst, 10000, false, true);
        removeInts<int>(bst, true, 10000, true);
        //timeTaken = clock() - start;
        //cout << endl <<  "Time taken: " << timeTaken << "ms" << endl; 
        break;
    default:
        cout << "Please select a valid test." << endl;
        break;
    }

    cout << "========================================" << endl;

    // delete the allocatorfind(ing) a few things in a BST
    delete allocator;

    return 0;
}
