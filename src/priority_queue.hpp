#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T value;
        Node* left;
        Node* right;
        int dist; // null path length

        Node(const T& val) : value(val), left(nullptr), right(nullptr), dist(0) {}
    };

    Node* root;
    size_t queue_size;
    Compare cmp;

    // Helper functions
    int getDist(Node* node) const {
        return node ? node->dist : -1;
    }

    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        // Ensure a has larger value (max-heap)
        try {
            if (cmp(a->value, b->value)) {
                std::swap(a, b);
            }
        } catch (...) {
            throw sjtu::runtime_error();
        }

        // Recursively merge
        try {
            a->right = merge(a->right, b);
        } catch (...) {
            // If merge fails, restore the right child
            a->right = nullptr;
            throw sjtu::runtime_error();
        }

        // Maintain leftist property: left dist >= right dist
        if (getDist(a->left) < getDist(a->right)) {
            std::swap(a->left, a->right);
        }

        // Update dist
        a->dist = getDist(a->right) + 1;
        return a;
    }

    Node* copyTree(Node* node) {
        if (!node) return nullptr;
        Node* new_node = new Node(node->value);
        new_node->dist = node->dist;
        new_node->left = copyTree(node->left);
        new_node->right = copyTree(node->right);
        return new_node;
    }

    void clearTree(Node* node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), queue_size(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(copyTree(other.root)), queue_size(other.queue_size) {}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
        clearTree(root);
    }

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
        if (this != &other) {
            clearTree(root);
            root = copyTree(other.root);
            queue_size = other.queue_size;
        }
        return *this;
    }

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->value;
    }

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
        Node* new_node = new Node(e);
        try {
            root = merge(root, new_node);
            ++queue_size;
        } catch (...) {
            delete new_node;
            throw sjtu::runtime_error();
        }
    }

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
        if (empty()) {
            throw container_is_empty();
        }

        Node* old_root = root;
        try {
            root = merge(root->left, root->right);
            delete old_root;
            --queue_size;
        } catch (...) {
            // If merge fails, restore the original root
            root = old_root;
            throw sjtu::runtime_error();
        }
    }

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
        return queue_size;
    }

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
        return queue_size == 0;
    }

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
        try {
            root = merge(root, other.root);
            queue_size += other.queue_size;
            other.root = nullptr;
            other.queue_size = 0;
        } catch (...) {
            throw sjtu::runtime_error();
        }
    }
};

}

#endif