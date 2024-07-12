/**
 * @file AVL.h
 * @author Chek
 * @brief This file contains the AVL tree class declaration
 * @date 27 Oct 2023
 */

#ifndef AVL_H
#define AVL_H
#include <iostream>
#include <sstream>
#include <stack>
#include "SimpleAllocator.h"
#include <vector>


/**
 * @brief AVL tree class
 *        Here, the suggested approach is to inherit from the BST class
 *        you have already completed in the DSA-BST assignment.
 *        However, it is up to you how you implement this class, as long as 
 *        the interface works as expected in test.cpp
 *        If you choose not to inherit from BST, you can remove all the 
 *        inheritance code, e.g., `: public BST<T>` from the class definition, 
 *        `virtual`/`override` modifiers from the inherited methods, etc.
 * @tparam T Type of data to be stored in the tree
 */
#include "BST.h"
template <typename T>
class AVL : public BST<T> {

public:

    // This is a convenience type definition for the stack of path nodes.
    // It is used to trace back to the unbalanced node(s) after adding/removing, 
    // as shown in class. Feel free to ignore this and use other techniques if you wish.
    using PathStack = std::stack<typename BST<T>::BinTree>;
    //using PathStack = std::vector<typename BST<T>::BinTree>;
    /**
     * @brief Constructor.
     *        The inline implementation here calls the BST constructor.
     *        However, you can implement this from scratch if you wish.
     * @param allocator Pointer to the allocator to use for the tree.
     */
    AVL(SimpleAllocator* allocator = nullptr) : BST<T>(allocator) {}

    /**
     * @brief Destructor
     *        The inline implementation here calls the BST destructor. 
     *        However, you can implement this from scratch if you wish.
     */
    virtual ~AVL() override {}

    /**
     * @brief Add a new value to the tree and balance the tree.
     *        Use the techniques discussed in class to trace back to the
     *        first unbalanced node and perform the appropriate rotations.
     *        Feel free to use other techniques if you wish but make sure your 
     *        outputs match the expected ones.
     *        (The declaration assumes you will override the remove method from BST. 
     *        However, you can remove the inheritance association if you wish.)
     * @param value to be added to the tree
     * @throw BSTException if the value already exists in the tree
     */
    virtual void add(const T& value) override;

    /**
     * @brief Remove a value from the tree and balance the tree.
     *        Use the techniques discussed in class to trace back to ALL
     *        unbalanced nodes and perform the appropriate rotations.
     *        Feel free to use other techniques if you wish but make sure your 
     *        outputs match the expected ones.
     *        (The declaration assumes you will override the remove method from BST. 
     *        However, you can remove the inheritance association if you wish.)
     * @param value to be removed from the tree
     * @throw BSTException if the value does not exist in the tree
     */
    virtual void remove(const T& value) override;

    /**
     * @brief Print the inorder traversal of the tree.
     * @return stringstream containing the inorder traversal of the tree
     */
    std::stringstream printInorder() const;

    /**
     * @brief Get the height of the tree.
     *        This is mainly used to update the cached balance factors in the nodes.
     *        It is also called in test.cpp to verify correctness.
     *        (The inline implementation defaults to the BST height method.
     *        However, you can implement this from scratch if you wish.)
     * @return height of the tree
     */
    int height() const {
        return BST<T>::height();
    }

    /**
     * @brief Get the size of the tree.
     *        This is mainly used to update the cached counts in the nodes.
     *        It is also called in test.cpp to verify correctness.
     *        (The inline implementation defaults to the BST size method. 
     *        However, you can implement this from scratch if you wish.)
     * @return size of the tree
     */
    unsigned size() const {
        return BST<T>::size();
    }
    void inOrder(PathStack& pathNodes);


private:

    // TODO: Add any private methods or data members you need here.
    //       For the public interface above, as mentioned, the main requirement
    //       is that the interface works as expected in test.cpp.

    /**
     * @brief Rotate the tree to the left.
     *        Note that you may need to update the counts and balance factors.
     * @param tree to be rotated
     */
    void rotateLeft(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the tree to the right.
     *        Note that you may need to update the counts and balance factors.
     * @param tree to be rotated
     */
    void rotateRight(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the left subtree to the left, then the whole tree to the right.
     *        You may want to use the rotateLeft and rotateRight methods above.
     * @param tree to be rotated
     */
    void rotateLeftRight(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the right subtree to the right, then the whole tree to the left.
     *        You may want to use the rotateLeft and rotateRight methods above.
     * @param tree to be rotated
     */
    void rotateRightLeft(typename BST<T>::BinTree& tree);

    /**
     * @brief Balance the tree from the stack of path nodes.
     * @param pathNodes contain the stack of nodes from the root to the node just before
     *                  the added/removed node after adding/remove operation
     * @param stopAtFirst if true, stop after fixing the first unbalanced node
     */
    void balance(PathStack& pathNodes, bool stopAtFirst = false);

    /**
     * @brief Balance the tree from the given node.
     *        This checks the balance factors of the node and its' children to 
     *        determine which of LL, LR, RR, RL case they are and call the
     *        appropriate rotation methods above.
     * @param tree to be balanced
     */
    void balancenode(typename BST<T>::BinTree& tree);

    int height_(const typename BST<T>::BinTree& tree) const;

    // TODO: Again, you do not need to stick to the private methods above, 
    //       and likely you will need to add more of your own methods in order
    //       to make your code more readable and maintainable.
    void printInorder_(const typename BST<T>::BinTree& tree, std::stringstream& ss) const;
    void add_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes);
    void remove_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes);
    bool isBalanced(const typename BST<T>::BinTree& tree) const;
    int getBalance(typename BST<T>::BinTree N);

    // void rotateLeftWithStatsUpdate(typename BST<T>::BinTree& tree);
    // void rotateRightWithStatsUpdate(typename BST<T>::BinTree& tree);
};

#include "AVL.cpp"

#endif // AVL_H

