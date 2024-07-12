#include "BST.h"

unsigned counter = 0;
// Constructor
template <typename T>
BST<T>::BST(SimpleAllocator* allocator) : allocator_(allocator), isOwnAllocator_(allocator == nullptr), root_(nullptr) {
    // Constructor implementation
}

// Copy constructor
template <typename T>
BST<T>::BST(const BST& rhs) {
    allocator_ = rhs.allocator_;
    isOwnAllocator_ = rhs.isOwnAllocator_;
    root_ = nullptr; // Initialize root for copying
    copy_(root_, rhs.root_);
}

// Assignment operator
template <typename T>
BST<T>& BST<T>::operator=(const BST& rhs) {
    if (this != &rhs) {
        clear();
        allocator_ = rhs.allocator_;
        copy_(root_, rhs.root_);
    }
    return *this;
}

// Destructor
template <typename T>
BST<T>::~BST() {
    clear();
}

// Subscript operator
template <typename T>
const typename BST<T>::BinTreeNode* BST<T>::operator[](int index) const
{
        if(this->size() <= (unsigned)index || index < 0){
        throw BSTException(1, "Index out of bounds");
    }else{
        return getNode_(root_, index);
    }
    return nullptr;
}

// Add a value to the tree
template <typename T>
void BST<T>::add(const T& value) noexcept(false) {
    add_(root_, value);
}

// Remove a value from the tree
template <typename T>
void BST<T>::remove(const T& value) {
    remove_(root_, value);
}

// Clear the tree
template <typename T>
void BST<T>::clear() {
    clear_(root_);
}

template <typename T>
void BST<T>::clear_(BinTree& tree) {
    if (tree == nullptr) {
        return;
    }
    clear_(tree->left);
    clear_(tree->right);
    freeNode(tree);
    tree = nullptr;
}
// Find a value in the tree
template <typename T>
bool BST<T>::find(const T& value, unsigned& compares) const {
    return find_(root_, value, compares);
}

// Check if the tree is empty
template <typename T>
bool BST<T>::empty() const {
    // Implement the empty function
    if (height() <= 0){
        return true;
    }
    return false;
}
// Get the number of nodes in the tree
template <typename T>
unsigned int BST<T>::size() const {
    return size_(root_);
}

// Get the height of the tree
template <typename T>
int BST<T>::height() const {
    return height_(root_);
}

// Get the root of the tree
template <typename T>
typename BST<T>::BinTree BST<T>::root() const {
    return root_;
}

// Allocate a new node
template <typename T>
typename BST<T>::BinTree BST<T>::makeNode(const T& value) {
    BinTree newNode = new BinTreeNode(value);
    return newNode;
}

// Free a node
template <typename T>
void BST<T>::freeNode(typename BST<T>::BinTree node) {
    delete node;
    
}

// Calculate the height of the tree
template <typename T>
int BST<T>::treeHeight(typename BST<T>::BinTree tree) const {
    if (tree == nullptr) {
        return -1; // Height of an empty tree
    }
    int leftHeight = treeHeight(tree->left);
    int rightHeight = treeHeight(tree->right);
    return std::max(leftHeight, rightHeight) + 1;
}

// Find the predecessor of a node
template <typename T>
void BST<T>::findPredecessor(typename BST<T>::BinTree tree, typename BST<T>::BinTree& predecessor) const {
    if (tree->left == nullptr) {
        return;
    }
    BinTree current = tree->left;
    while (current->right != nullptr) {
        current = current->right;
    }
    predecessor = current;
}

// Check if the tree is empty
template <typename T>
bool BST<T>::isEmpty(const typename BST<T>::BinTree& tree) const {
    if(tree ==nullptr)
    {
        return true;
    }
    else{
        return false;
    }
    
}


// Check if the tree is a leaf
template <typename T>
bool BST<T>::isLeaf(const typename BST<T>::BinTree& tree) const {
    return tree != nullptr && tree->left == nullptr && tree->right == nullptr;
}

// A recursive step to add a value into the tree
template <typename T>
void BST<T>::add_(typename BST<T>::BinTree& tree, const T& value) {
    if (tree == nullptr) {
        tree = makeNode(value);
        return;
    }
    if (value < tree->data) {
        add_(tree->left, value);
    } else if (value > tree->data) {
        add_(tree->right, value);
    } else {
        // Value already exists, throw an exception
        throw BSTException(BSTException::E_DUPLICATE, "Value already exists in the tree");
    }
}

// A recursive step to find the value in the tree
template <typename T>
bool BST<T>::find_(const typename BST<T>::BinTree& tree, const T& value, unsigned& compares) const {
    if (isEmpty(tree)==true) { //check if tree node is 
        return false;
    }
    if (value < tree->data) {
        compares++; // Increment comparison count
        return find_(tree->left, value, compares);
    } else if (value > tree->data) {
        compares++; // Increment comparison count
        return find_(tree->right, value, compares);

    } else if(value == tree->data) {
        compares++; // Increment comparison count
        return true; // Value found
    } else {
        return false;
    }
}

// A recursive step to get to the node at the specified index
template <typename T>
const typename BST<T>::BinTree BST<T>::getNode_(const typename BST<T>::BinTree& tree, int index) const {
    if (tree == nullptr || index < 0) {
        // Index out of range or empty tree
        throw BSTException(BSTException::E_OUT_BOUNDS, "Index is out of range or tree is empty");
    }
    int leftSubtreeSize = size_(tree->left);
    if (index < leftSubtreeSize) {
        // The node is in the left subtree
        return getNode_(tree->left, index);
    } else if (index == leftSubtreeSize) {
        // The current node is the one at the specified index
        return tree;
    } else {
        // The node is in the right subtree
        return getNode_(tree->right, index - leftSubtreeSize - 1);
    }
}

// A recursive step to calculate the size of the tree
template <typename T>
unsigned BST<T>::size_(const typename BST<T>::BinTree& tree) const {
    if (tree == nullptr) {
        return 0;
    }

    // Recursively count the nodes in the left and right subtrees
    int leftSize = size_(tree->left);
    int rightSize = size_(tree->right);
    // Return the total size, which is the sum of the left and right subtree sizes plus 1 for the current node
    return leftSize + rightSize + 1;
}


// A recursive step to remove a value from the tree
template <typename T>
void BST<T>::remove_(typename BST<T>::BinTree& tree, const T& value) {
    if (tree == nullptr) {
        // Value not found, throw an exception
        throw BSTException(BSTException::E_NOT_FOUND, "Value to remove not found in the tree");
    }
    if (value < tree->data) {
        remove_(tree->left, value);
    } else if (value > tree->data) {
        remove_(tree->right, value);
    } else {
        // Value found, determine the case
        if (tree->left == nullptr) {
            // Case 1: No left child or both children are nullptr
            BinTree temp = tree->right;
            freeNode(tree);
            tree = temp;
        } else if (tree->right == nullptr) {
            // Case 2: No right child
            BinTree temp = tree->left;
            freeNode(tree);
            tree = temp;
        } else {
            // Case 3: Node has two children
            BinTree predecessor = nullptr;
            findPredecessor(tree, predecessor);
            tree->data = predecessor->data;
            remove_(tree->left, predecessor->data);
        }
    }
}

// A recursive step to calculate the height of the tree
template <typename T>
int BST<T>::height_(const typename BST<T>::BinTree& tree) const {
    return treeHeight(tree);
}

// A recursive step to copy the tree
template <typename T>
void BST<T>::copy_(typename BST<T>::BinTree& tree, const typename BST<T>::BinTree& rtree) {
    if (rtree == nullptr) {
        tree = nullptr;
    } else {
        tree = makeNode(rtree->data);
        copy_(tree->left, rtree->left);
        copy_(tree->right, rtree->right);
    }
}

// Explicit instantiation for the types you plan to use
template class BST<int>; // Example for an integer BST
