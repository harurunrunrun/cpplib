#ifndef CPPLIB_SRC_STRUCTURE_BBST_SET_AVL_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SET_AVL_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

template<class Key, class Compare = std::less<Key>>
class AvlTree {
    struct Node {
        Key key;
        int height = 1;
        std::size_t size = 1;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(Key key_) : key(std::move(key_)) {}
    };
    std::unique_ptr<Node> root_;
    Compare compare_;

    static int height(const std::unique_ptr<Node>& node) {
        return node ? node->height : 0;
    }
    static std::size_t subtree_size(const std::unique_ptr<Node>& node) {
        return node ? node->size : 0;
    }
    static void update(Node* node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
        node->size =
            1 + subtree_size(node->left) + subtree_size(node->right);
    }
    static void rotate_left(std::unique_ptr<Node>& node) {
        auto right = std::move(node->right);
        node->right = std::move(right->left);
        update(node.get());
        right->left = std::move(node);
        update(right.get());
        node = std::move(right);
    }
    static void rotate_right(std::unique_ptr<Node>& node) {
        auto left = std::move(node->left);
        node->left = std::move(left->right);
        update(node.get());
        left->right = std::move(node);
        update(left.get());
        node = std::move(left);
    }
    static void balance(std::unique_ptr<Node>& node) {
        if (!node) return;
        update(node.get());
        if (height(node->left) - height(node->right) == 2) {
            if (height(node->left->left) < height(node->left->right)) {
                rotate_left(node->left);
            }
            rotate_right(node);
        } else if (height(node->right) - height(node->left) == 2) {
            if (height(node->right->right) < height(node->right->left)) {
                rotate_right(node->right);
            }
            rotate_left(node);
        }
    }
    bool insert_impl(std::unique_ptr<Node>& node, const Key& key) {
        if (!node) {
            node = std::make_unique<Node>(key);
            return true;
        }
        bool inserted = false;
        if (compare_(key, node->key)) {
            inserted = insert_impl(node->left, key);
        } else if (compare_(node->key, key)) {
            inserted = insert_impl(node->right, key);
        }
        if (inserted) balance(node);
        return inserted;
    }
    static Key extract_min(std::unique_ptr<Node>& node) {
        if (!node->left) {
            Key result = std::move(node->key);
            node = std::move(node->right);
            return result;
        }
        Key result = extract_min(node->left);
        balance(node);
        return result;
    }
    bool erase_impl(std::unique_ptr<Node>& node, const Key& key) {
        if (!node) return false;
        bool erased = false;
        if (compare_(key, node->key)) {
            erased = erase_impl(node->left, key);
        } else if (compare_(node->key, key)) {
            erased = erase_impl(node->right, key);
        } else {
            erased = true;
            if (!node->left) node = std::move(node->right);
            else if (!node->right) node = std::move(node->left);
            else node->key = extract_min(node->right);
        }
        if (erased) balance(node);
        return erased;
    }

public:
    AvlTree() = default;
    explicit AvlTree(Compare compare) : compare_(std::move(compare)) {}
    [[nodiscard]] bool empty() const noexcept { return !root_; }
    [[nodiscard]] std::size_t size() const noexcept {
        return subtree_size(root_);
    }
    [[nodiscard]] bool contains(const Key& key) const {
        const Node* node = root_.get();
        while (node) {
            if (compare_(key, node->key)) node = node->left.get();
            else if (compare_(node->key, key)) node = node->right.get();
            else return true;
        }
        return false;
    }
    bool insert(const Key& key) { return insert_impl(root_, key); }
    bool erase(const Key& key) { return erase_impl(root_, key); }
    [[nodiscard]] std::optional<Key> lower_bound(const Key& key) const {
        const Node* node = root_.get();
        const Node* candidate = nullptr;
        while (node) {
            if (!compare_(node->key, key)) {
                candidate = node;
                node = node->left.get();
            } else node = node->right.get();
        }
        if (!candidate) return std::nullopt;
        return candidate->key;
    }
    [[nodiscard]] std::optional<Key> kth(std::size_t index) const {
        const Node* node = root_.get();
        while (node) {
            const std::size_t left_size = subtree_size(node->left);
            if (index < left_size) node = node->left.get();
            else if (index == left_size) return node->key;
            else {
                index -= left_size + 1;
                node = node->right.get();
            }
        }
        return std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SET_AVL_TREE_HPP_INCLUDED
