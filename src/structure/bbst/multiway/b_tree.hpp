#ifndef CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_TREE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

template<class Key, std::size_t MIN_DEGREE = 16,
         class Compare = std::less<Key>>
class BTree {
    static_assert(MIN_DEGREE >= 2);
    struct Node {
        bool leaf;
        std::vector<Key> keys;
        std::vector<std::unique_ptr<Node>> children;
        explicit Node(bool leaf_) : leaf(leaf_) {
            keys.reserve(2 * MIN_DEGREE - 1);
            children.reserve(2 * MIN_DEGREE);
        }
    };
    std::unique_ptr<Node> root_ = std::make_unique<Node>(true);
    std::size_t size_ = 0;
    Compare compare_;

    [[nodiscard]] bool equal(const Key& a, const Key& b) const {
        return !compare_(a, b) && !compare_(b, a);
    }
    std::size_t position(const Node& node, const Key& key) const {
        std::size_t index = 0;
        while (index < node.keys.size() && compare_(node.keys[index], key)) {
            ++index;
        }
        return index;
    }
    void split_child(Node& parent, std::size_t index) {
        Node* full = parent.children[index].get();
        auto right = std::make_unique<Node>(full->leaf);
        Key middle = std::move(full->keys[MIN_DEGREE - 1]);
        for (std::size_t i = MIN_DEGREE; i < full->keys.size(); ++i) {
            right->keys.push_back(std::move(full->keys[i]));
        }
        full->keys.resize(MIN_DEGREE - 1);
        if (!full->leaf) {
            for (std::size_t i = MIN_DEGREE; i < full->children.size(); ++i) {
                right->children.push_back(std::move(full->children[i]));
            }
            full->children.resize(MIN_DEGREE);
        }
        parent.keys.insert(
            parent.keys.begin() + static_cast<std::ptrdiff_t>(index),
            std::move(middle)
        );
        parent.children.insert(
            parent.children.begin() + static_cast<std::ptrdiff_t>(index + 1),
            std::move(right)
        );
    }
    void insert_nonfull(Node& node, const Key& key) {
        std::size_t index = position(node, key);
        if (node.leaf) {
            node.keys.insert(
                node.keys.begin() + static_cast<std::ptrdiff_t>(index), key
            );
            return;
        }
        if (node.children[index]->keys.size() == 2 * MIN_DEGREE - 1) {
            split_child(node, index);
            if (compare_(node.keys[index], key)) ++index;
        }
        insert_nonfull(*node.children[index], key);
    }
    static Key maximum_key(Node* node) {
        while (!node->leaf) node = node->children.back().get();
        return node->keys.back();
    }
    static Key minimum_key(Node* node) {
        while (!node->leaf) node = node->children.front().get();
        return node->keys.front();
    }
    void merge_children(Node& node, std::size_t index) {
        auto right = std::move(node.children[index + 1]);
        Node* left = node.children[index].get();
        left->keys.push_back(std::move(node.keys[index]));
        for (auto& key : right->keys) left->keys.push_back(std::move(key));
        for (auto& child : right->children) {
            left->children.push_back(std::move(child));
        }
        node.keys.erase(
            node.keys.begin() + static_cast<std::ptrdiff_t>(index)
        );
        node.children.erase(
            node.children.begin() + static_cast<std::ptrdiff_t>(index + 1)
        );
    }
    void borrow_previous(Node& node, std::size_t index) {
        Node* child = node.children[index].get();
        Node* sibling = node.children[index - 1].get();
        child->keys.insert(child->keys.begin(), std::move(node.keys[index - 1]));
        node.keys[index - 1] = std::move(sibling->keys.back());
        sibling->keys.pop_back();
        if (!child->leaf) {
            child->children.insert(
                child->children.begin(), std::move(sibling->children.back())
            );
            sibling->children.pop_back();
        }
    }
    void borrow_next(Node& node, std::size_t index) {
        Node* child = node.children[index].get();
        Node* sibling = node.children[index + 1].get();
        child->keys.push_back(std::move(node.keys[index]));
        node.keys[index] = std::move(sibling->keys.front());
        sibling->keys.erase(sibling->keys.begin());
        if (!child->leaf) {
            child->children.push_back(std::move(sibling->children.front()));
            sibling->children.erase(sibling->children.begin());
        }
    }
    void fill_child(Node& node, std::size_t index) {
        if (index > 0
            && node.children[index - 1]->keys.size() >= MIN_DEGREE) {
            borrow_previous(node, index);
        } else if (index + 1 < node.children.size()
                   && node.children[index + 1]->keys.size() >= MIN_DEGREE) {
            borrow_next(node, index);
        } else if (index + 1 < node.children.size()) {
            merge_children(node, index);
        } else {
            merge_children(node, index - 1);
        }
    }
    bool erase_impl(Node& node, const Key& key) {
        std::size_t index = position(node, key);
        if (index < node.keys.size() && equal(node.keys[index], key)) {
            if (node.leaf) {
                node.keys.erase(
                    node.keys.begin() + static_cast<std::ptrdiff_t>(index)
                );
                return true;
            }
            if (node.children[index]->keys.size() >= MIN_DEGREE) {
                Key replacement = maximum_key(node.children[index].get());
                node.keys[index] = replacement;
                return erase_impl(*node.children[index], replacement);
            }
            if (node.children[index + 1]->keys.size() >= MIN_DEGREE) {
                Key replacement = minimum_key(node.children[index + 1].get());
                node.keys[index] = replacement;
                return erase_impl(*node.children[index + 1], replacement);
            }
            merge_children(node, index);
            return erase_impl(*node.children[index], key);
        }
        if (node.leaf) return false;
        const bool was_last = index == node.keys.size();
        if (node.children[index]->keys.size() < MIN_DEGREE) {
            fill_child(node, index);
            if (was_last && index > node.keys.size()) --index;
        }
        return erase_impl(*node.children[index], key);
    }

public:
    BTree() = default;
    explicit BTree(Compare compare) : compare_(std::move(compare)) {}
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(const Key& key) const {
        const Node* node = root_.get();
        while (true) {
            const std::size_t index = position(*node, key);
            if (index < node->keys.size() && equal(node->keys[index], key)) {
                return true;
            }
            if (node->leaf) return false;
            node = node->children[index].get();
        }
    }
    bool insert(const Key& key) {
        if (contains(key)) return false;
        if (root_->keys.size() == 2 * MIN_DEGREE - 1) {
            auto new_root = std::make_unique<Node>(false);
            new_root->children.push_back(std::move(root_));
            split_child(*new_root, 0);
            root_ = std::move(new_root);
        }
        insert_nonfull(*root_, key);
        ++size_;
        return true;
    }
    bool erase(const Key& key) {
        if (!erase_impl(*root_, key)) return false;
        --size_;
        if (!root_->leaf && root_->keys.empty()) {
            root_ = std::move(root_->children.front());
        }
        return true;
    }
    [[nodiscard]] std::optional<Key> lower_bound(const Key& key) const {
        const Node* node = root_.get();
        std::optional<Key> answer;
        while (node) {
            const std::size_t index = position(*node, key);
            if (index < node->keys.size()) answer = node->keys[index];
            if (node->leaf) break;
            node = node->children[index].get();
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_TREE_HPP_INCLUDED
