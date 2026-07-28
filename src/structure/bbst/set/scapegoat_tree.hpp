#ifndef CPPLIB_SRC_STRUCTURE_BBST_SET_SCAPEGOAT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SET_SCAPEGOAT_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

template<class Key, class Compare = std::less<Key>>
class ScapegoatTree {
    struct Node {
        Key key;
        std::size_t size = 1;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(Key key_) : key(std::move(key_)) {}
    };
    std::unique_ptr<Node> root_;
    Compare compare_;
    static constexpr std::size_t ALPHA_NUMERATOR = 3;
    static constexpr std::size_t ALPHA_DENOMINATOR = 4;

    static std::size_t size_of(const std::unique_ptr<Node>& node) {
        return node ? node->size : 0;
    }
    static void update(Node* node) {
        node->size = 1 + size_of(node->left) + size_of(node->right);
    }
    static void flatten(std::unique_ptr<Node> node,
                        std::vector<std::unique_ptr<Node>>& nodes) {
        if (!node) return;
        auto left = std::move(node->left);
        auto right = std::move(node->right);
        flatten(std::move(left), nodes);
        node->left.reset();
        node->right.reset();
        nodes.push_back(std::move(node));
        flatten(std::move(right), nodes);
    }
    static std::unique_ptr<Node> build(
        std::vector<std::unique_ptr<Node>>& nodes,
        std::size_t left, std::size_t right
    ) {
        if (left == right) return nullptr;
        const std::size_t middle = left + (right - left) / 2;
        auto node = std::move(nodes[middle]);
        node->left = build(nodes, left, middle);
        node->right = build(nodes, middle + 1, right);
        update(node.get());
        return node;
    }
    static void rebuild(std::unique_ptr<Node>& node) {
        std::vector<std::unique_ptr<Node>> nodes;
        nodes.reserve(node->size);
        flatten(std::move(node), nodes);
        node = build(nodes, 0, nodes.size());
    }
    static bool unbalanced(const Node* node) {
        const std::size_t larger =
            std::max(size_of(node->left), size_of(node->right));
        return larger * ALPHA_DENOMINATOR
            > node->size * ALPHA_NUMERATOR;
    }
    bool insert_impl(std::unique_ptr<Node>& node, const Key& key) {
        if (!node) {
            node = std::make_unique<Node>(key);
            return true;
        }
        bool inserted = false;
        if (compare_(key, node->key)) inserted = insert_impl(node->left, key);
        else if (compare_(node->key, key)) {
            inserted = insert_impl(node->right, key);
        }
        if (inserted) {
            update(node.get());
            if (unbalanced(node.get())) rebuild(node);
        }
        return inserted;
    }
    static Key extract_min(std::unique_ptr<Node>& node) {
        if (!node->left) {
            Key result = std::move(node->key);
            node = std::move(node->right);
            return result;
        }
        Key result = extract_min(node->left);
        update(node.get());
        if (unbalanced(node.get())) rebuild(node);
        return result;
    }
    bool erase_impl(std::unique_ptr<Node>& node, const Key& key) {
        if (!node) return false;
        bool erased = false;
        if (compare_(key, node->key)) erased = erase_impl(node->left, key);
        else if (compare_(node->key, key)) {
            erased = erase_impl(node->right, key);
        } else {
            erased = true;
            if (!node->left) node = std::move(node->right);
            else if (!node->right) node = std::move(node->left);
            else node->key = extract_min(node->right);
        }
        if (erased && node) {
            update(node.get());
            if (unbalanced(node.get())) rebuild(node);
        }
        return erased;
    }

public:
    ScapegoatTree() = default;
    explicit ScapegoatTree(Compare compare) : compare_(std::move(compare)) {}
    [[nodiscard]] bool empty() const noexcept { return !root_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_of(root_); }
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
        return candidate ? std::optional<Key>(candidate->key) : std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SET_SCAPEGOAT_TREE_HPP_INCLUDED
