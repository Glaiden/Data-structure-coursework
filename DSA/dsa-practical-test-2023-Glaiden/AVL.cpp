/*@brief
  @author - Tan Choon Yi
  @admin number- 2201766
  @test- practical test DSA 
*/
#include "AVL.h"
#include <iostream>
#include <sstream>
#include <stack>
#include "SimpleAllocator.h"
#include "BST.h"


// function that calls add()
template <typename T>
void AVL<T>::add(const T& value) {
    PathStack pathNodes;
    add_(this->rootRef(), value, pathNodes);
    
}

// function that calls remove_()
template <typename T>
void AVL<T>::remove(const T& value) {
    PathStack pathNodes;
    remove_(this->rootRef(),value,pathNodes);
}
//adding new node to hold input value 
template <typename T>
void AVL<T>::add_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes)
{
    //if tree is empty make node to hold value
    if (!tree) {
        tree = BST<T>::makeNode(value);
    } else if (value < tree->data) { // if input value is less than current node data 
        pathNodes.push(tree);  // Push the parent node onto the stack
        add_(tree->left, value, pathNodes); // recursively call add_() but use left child node of current node as input to hold value
    } else {
        pathNodes.push(tree);  // Push the parent node onto the stack
        add_(tree->right, value, pathNodes);
    }
    balancenode(tree); // balance the tree using current node at reference

}
// removes node holding a ccertain input value
template <typename T>
void AVL<T>::remove_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes) {
    pathNodes.push(tree); // push parent node into stack
    BST<T>::remove(value); // calls remove() from BST 
    pathNodes.pop();  // Pop the parent node
    tree->height_ = 1 + std::max(BST<T>::treeHeight(tree->left), BST<T>::treeHeight(tree->right)); // update height
    balancenode(tree); // balance the tree using current node at reference

}
//balance function that access stack to balance(does not work)
template <typename T>
void AVL<T>::balance(PathStack& pathNodes, bool stopAtFirst) {
    bool balanced = false;
    while (!pathNodes.empty()) {
        typename BST<T>::BinTree node = pathNodes.top();
        pathNodes.pop();

        balancenode(node);
        if (stopAtFirst && balanced) break;

        if (!balanced && std::abs(getBalance(node)) > 1) {
            balanced = true;
        }
    }
}

//balance function that determine rotation based on balancefactor
template <typename T>
void AVL<T>::balancenode(typename BST<T>::BinTree& tree) {
        tree->height_ = 1 + std::max(BST<T>::treeHeight(tree->left), BST<T>::treeHeight(tree->right));

        // Calculate the balance factor
        int balanceFactor = BST<T>::treeHeight(tree->left) - BST<T>::treeHeight(tree->right);

        // Perform rotations if necessary
        if (balanceFactor > 1) {
            if (BST<T>::treeHeight(tree->left->left) >= BST<T>::treeHeight(tree->left->right)) {
                rotateRight(tree);
            } else {
                rotateLeftRight(tree);
            }
        } else if (balanceFactor < -1) {
            if (BST<T>::treeHeight(tree->right->right) >= BST<T>::treeHeight(tree->right->left)) {
                rotateLeft(tree);
            } else {
                rotateRightLeft(tree);
            }
        }
    }

// Utility function to get the balance factor of the node
template <typename T>
int AVL<T>::getBalance(typename BST<T>::BinTree N) {
    if (N == nullptr)
        return 0;
    return BST<T>::treeHeight(N->left) - BST<T>::treeHeight(N->right);
}



template <typename T>
void AVL<T>::rotateLeft(typename BST<T>::BinTree& tree) {
    if (!tree || !tree->right) {
        return; // Nothing to rotate
    }
    // Create a new root node, set its left child to the current root,
    // and update the current root's right child.
    typename BST<T>::BinTree newRoot = tree->right;
    tree->right = newRoot->left;
    newRoot->left = tree;

    // Update the height of tree and newRoot.
    tree->height_ = 1 + std::max(BST<T>::treeHeight(tree->left), BST<T>::treeHeight(tree->right));
    newRoot->height_ = 1 + std::max(BST<T>::treeHeight(newRoot->left), BST<T>::treeHeight(newRoot->right));

    tree = newRoot; // Update the root.
}
template <typename T>
void AVL<T>::rotateRight(typename BST<T>::BinTree& tree) {
    typename BST<T>::BinTree leftSubtree = tree->left;
    tree->left = leftSubtree->right;
    leftSubtree->right = tree;
    BST<T>::treeHeight(tree);
    BST<T>::treeHeight(leftSubtree);
    tree = leftSubtree;
}

template <typename T>
void AVL<T>::rotateLeftRight(typename BST<T>::BinTree& tree) {
    rotateLeft(tree->left);
    rotateRight(tree);
}

template <typename T>
void AVL<T>::rotateRightLeft(typename BST<T>::BinTree& tree) {
    rotateRight(tree->right);
    rotateLeft(tree);
}

template <typename T>
std::stringstream AVL<T>::printInorder() const {
    std::stringstream ss;
    printInorder_(this->root(), ss);
    return ss;
}

template <typename T>
void AVL<T>::printInorder_(const typename BST<T>::BinTree& tree, std::stringstream& ss) const {
    if (tree) {
        printInorder_(tree->left, ss);
        ss << tree->data << " ";
        printInorder_(tree->right, ss);
    }
}


template <typename T>
bool AVL<T>::isBalanced(const typename BST<T>::BinTree& tree) const {
    if (!tree) return true; // A nullptr tree is balanced
    
    int leftHeight = tree->left ? tree->left->height_ : 0;
    int rightHeight = tree->right ? tree->right->height_ : 0;
    
    return abs(leftHeight - rightHeight) <= 1 
           && isBalanced(tree->left) 
           && isBalanced(tree->right);
}

