/**
 * @file BST.h
 * @author Chek
 * @brief BST class definition
 *        Note that this file is lengthy
 *        as it tries to contain as much information as possible
 *        for students to understand the requirements
 * @date 12 Sep 2023
 */
#ifndef BST_H
#define BST_H
#include "SimpleAllocator.h" // to use your SimpleAllocator
#include <stdexcept>
#include <string>

/**
 * @class BSTException
 * @brief Exception class for BST
 *       Derived from std::exception
 *       It is used to throw exceptions when an error occurs
 *       in the BST class
 */
class BSTException : public std::exception {
  public:
    BSTException(int ErrCode, const std::string& Message)
        : error_code_(ErrCode), message_(Message){};

    enum BST_EXCEPTION { E_OUT_BOUNDS, E_DUPLICATE, E_NO_MEMORY, E_NOT_FOUND };

    virtual int code() const { return error_code_; }
    virtual const char* what() const throw() { return message_.c_str(); }
    virtual ~BSTException() throw() {}

  private:
    int error_code_;
    std::string message_;
};

/**
 * @class BST
 * @brief Binary Search Tree class
 *       It is a template class
 *       It is implemented using a linked structure
 *       It is not a balanced tree
 */
template <typename T> 
class BST {
  public:
    /**
     * @struct BinTreeNode
     * @brief A node in the Binary Tree
     *        - didn't name it BSTNode as the definition here is generic
     */
struct BinTreeNode
{
    BinTreeNode* left;
    BinTreeNode* right;
    T data;
    unsigned count;
    int height_; // Added this line

    // default constructor
    BinTreeNode()
        : left(nullptr), right(nullptr), data(0), count(0), height_(1) {}; // Updated this line

    // constructor with data
    BinTreeNode(const T& value)
        : left(nullptr), right(nullptr), data(value), count(0), height_(1) {}; // Updated this line
    };
    typedef BinTreeNode* BinTree; // BinTree is a pointer to BinTreeNode

    /**
     * @brief Default constructor
     * @param allocator The allocator to be used
     */
    BST(SimpleAllocator* allocator = nullptr);

    /**
     * @brief Copy constructor
     * @param rhs The BST to be copied
     */
    BST(const BST& rhs);

    /**
     * @brief Assignment operator
     * @param rhs The BST to be copied
     */
    BST& operator=(const BST& rhs);

    /**
     * @brief Destructor
     *        It calls clear() to free all nodes
     *        It is virtual so that the destructor of the derived class
     *        will be called when appropriate
     */
    virtual ~BST();

    /**
     * @brief Subscript operator thatreturns the node at the specified index.
     *        It calls getNode_() to do the actual recursive traversal
     * @param index The index of the node to be returned
     * @return The node at the specified index
     * @throw BSTException if the index is out of range
     * TODO change to return BinTree&
     */
    const BinTreeNode* operator[](int index) const;

    /**
     * @brief Insert a value into the tree
     *        It calls add_() to do the actual recursive insertion
     *        It is virtual so that any derived class knows to override it
     * @param value The value to be added
     * @throw BSTException if the value already exists
     */
    virtual void add(const T& value) noexcept(false);

    /**
     * @brief Remove a value from the tree
     *        It calls remove_() to do the actual recursive removal
     *        It is virtual so that any derived class knows to override it
     * @param value The value to be removed
     * @throw BSTException if the value does not exist
     */
    virtual void remove(const T& value);

    /**
     * @brief Remove all nodes in the tree
     */
    void clear();

    /**
     * @brief Find a value in the tree
     *        It calls find_() to do the actual recursive search
     * @param value The value to be found
     * @param compares The number of comparisons made 
     *                 (a reference to provide as output)
     * @return true if the value is found
     *         false otherwise
     */
    bool find(const T& value, unsigned& compares) const;

    /**
     * @brief Check if the tree is empty
     * @return true if the tree is empty
     *         false otherwise
     */
    bool empty() const;

    /**
     * @brief Get the number of nodes in the tree
     * @return The number of nodes in the tree
     */
    unsigned int size() const;

    /**
     * @brief Get the height of the tree
     *        It calls height_() to do the actual recursive calculation
     * @return The height of the tree
     */
    int height() const;

    /**
     * @brief Get the root of the tree
     * @return The root of the tree
     */
    BinTree root() const;

  protected:

    /**
     * @brief Allocate a new node
     * @param value The value to be stored in the new node
     */
    BinTree makeNode(const T& value);

    /**
     * @brief Free a node
     * @param node The node to be freed
     */
    void freeNode(BinTree node);

    /**
     * @brief Calculate the height of the tree
     * @param tree The tree to be calculated
     */
    int treeHeight(BinTree tree) const;

    /**
     * @brief Find the predecessor of a node
     * @param tree The tree to be searched
     * @param predecessor The predecessor of the node
     */
    void findPredecessor(BinTree tree, BinTree& predecessor) const;

    /**
     * @brief Check if the tree is empty
     * @param tree The tree to be checked
     * @return true if the tree is empty
     *         false otherwise
     */
    //bool isEmpty(BinTree& tree) const;
    bool isEmpty(const BinTree& tree) const;

    /**
     * @brief Check if the tree is a leaf
     * @param tree The tree to be checked
     * @return true if the tree is a leaf
     *         false otherwise
     */
    bool isLeaf(const BinTree& tree) const;
    // Accessor to get a reference to root_
    BinTree& rootRef() { return root_; }
    int height_(const BinTree& tree) const;
    // ... rest of the protected members ...

  private:

    // the allocator to be used
    SimpleAllocator* allocator_;

    // whether the allocator is owned by the tree
    bool isOwnAllocator_ = false;

    // the root of the tree
    BinTree root_;

    /**
     * @brief A recursive step to add a value into the tree
     * @param tree The tree to be added
     * @param value The value to be added
     */
    void add_(BinTree& tree, const T& value);

    /**
     * @brief A recursive step to find the value in the tree
     * @param tree The tree to be searched
     * @param value The value to be found
     * @param compares The number of comparisons made
     *                (a reference to provide as output)
     *                Note that it is not const as it is used to provide output
     * @return true if the value is found
     */
    bool find_(const BinTree& tree, const T& value, unsigned& compares) const;

    /**
     * @brief A recursive step to get to the node at the specified index
     *        This is used by operator[]
     * @param tree The tree to be traversed
     * @param index The index of the node to be returned
     * @return The node at the specified index
     * @throw BSTException if the index is out of range
     */
     const BinTree getNode_(const BinTree& tree, int index) const;

    /**
     * @brief A recursive step to calculate the size of the tree
     * @param tree The tree to be calculated
     * @return The size of the tree
     */
    unsigned size_(const BinTree& tree) const;

    /**
     * @brief A recursive step to remove a value from the tree
     * @param tree The tree to be removed
     * @param value The value to be removed
     */
    void remove_(BinTree& tree, const T& value);

    /**
     * @brief A recursive step to calculate the height of the tree
     * @param tree The tree to be calculated
     * @return The height of the tree
     */


    /**
     * @brief A recursive step to copy the tree 
     * @param tree The tree to be copied
     * @param rtree The tree to be copied to
     */
    void copy_(BinTree& tree, const BinTree& rtree);
 
    void clear_(BinTree& tree);
};

// This is the header file but it is including the implemention cpp because
// is a templated class. In C++, the compiler needs to know the implementation
// of the class in order to compile it. The compiler does not know what type
// the class will be used for. Therefore, the implementation is included in
// the header file. This is not a good practice as it will increase the
// compilation time. However, it is necessary for templated classes.
// It will show an err if the cpp file does not exist yet.
#include "BST.cpp"

#endif
