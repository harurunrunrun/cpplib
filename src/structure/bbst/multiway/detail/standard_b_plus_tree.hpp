#ifndef CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_DETAIL_STANDARD_B_PLUS_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_DETAIL_STANDARD_B_PLUS_TREE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Key, class Value, std::size_t FANOUT = 32,
         class Compare = std::less<Key>>
class StandardBPlusTree {
    static_assert(FANOUT >= 4);
    static constexpr std::size_t MIN_LEAF_KEYS = FANOUT / 2;
    static constexpr std::size_t MIN_INTERNAL_CHILDREN = (FANOUT + 1) / 2;

    struct Node {
        bool leaf;
        std::vector<Key> keys;
        std::vector<Value> values;
        std::vector<std::unique_ptr<Node>> children;
        Node* next = nullptr;
        std::optional<Key> minimum;
        explicit Node(bool leaf_) : leaf(leaf_) {
            keys.reserve(FANOUT);
            if (leaf) values.reserve(FANOUT);
            else children.reserve(FANOUT + 1);
        }
    };

    struct Split {
        std::unique_ptr<Node> right;
    };

    std::unique_ptr<Node> root_ = std::make_unique<Node>(true);
    std::size_t size_ = 0;
    Compare compare_;

    [[nodiscard]] bool equal(const Key& a, const Key& b) const {
        return !compare_(a, b) && !compare_(b, a);
    }

    std::size_t lower_position(
        const std::vector<Key>& keys, const Key& key
    ) const {
        std::size_t left = 0;
        std::size_t right = keys.size();
        while (left < right) {
            const std::size_t middle = (left + right) / 2;
            if (compare_(keys[middle], key)) left = middle + 1;
            else right = middle;
        }
        return left;
    }

    std::size_t child_position(
        const std::vector<Key>& separators, const Key& key
    ) const {
        std::size_t left = 0;
        std::size_t right = separators.size();
        while (left < right) {
            const std::size_t middle = (left + right) / 2;
            if (!compare_(key, separators[middle])) left = middle + 1;
            else right = middle;
        }
        return left;
    }

    static void refresh(Node& node) {
        if (node.leaf) {
            if (node.keys.empty()) node.minimum.reset();
            else node.minimum = node.keys.front();
            return;
        }
        if (node.children.empty()) {
            throw std::logic_error("BPlusTree internal node without children");
        }
        node.minimum = node.children.front()->minimum;
        node.keys.clear();
        node.keys.reserve(node.children.size() - 1);
        for (std::size_t index = 1; index < node.children.size(); ++index) {
            if (!node.children[index]->minimum) {
                throw std::logic_error("BPlusTree empty non-root child");
            }
            node.keys.push_back(*node.children[index]->minimum);
        }
    }

    std::optional<Split> insert_impl(
        Node& node, const Key& key, const Value& value, bool& inserted
    ) {
        if (node.leaf) {
            const std::size_t index = lower_position(node.keys, key);
            if (index < node.keys.size() && equal(node.keys[index], key)) {
                node.values[index] = value;
                inserted = false;
                return std::nullopt;
            }
            node.keys.insert(
                node.keys.begin() + static_cast<std::ptrdiff_t>(index), key
            );
            node.values.insert(
                node.values.begin() + static_cast<std::ptrdiff_t>(index), value
            );
            inserted = true;
            refresh(node);
            if (node.keys.size() < FANOUT) return std::nullopt;

            const std::size_t middle = node.keys.size() / 2;
            auto right = std::make_unique<Node>(true);
            right->keys.assign(
                std::make_move_iterator(node.keys.begin() + middle),
                std::make_move_iterator(node.keys.end())
            );
            right->values.assign(
                std::make_move_iterator(node.values.begin() + middle),
                std::make_move_iterator(node.values.end())
            );
            node.keys.resize(middle);
            node.values.resize(middle);
            right->next = node.next;
            node.next = right.get();
            refresh(node);
            refresh(*right);
            return Split{std::move(right)};
        }

        const std::size_t index = child_position(node.keys, key);
        auto split = insert_impl(*node.children[index], key, value, inserted);
        if (split) {
            node.children.insert(
                node.children.begin()
                    + static_cast<std::ptrdiff_t>(index + 1),
                std::move(split->right)
            );
        }
        refresh(node);
        if (node.children.size() <= FANOUT) return std::nullopt;

        const std::size_t middle = node.children.size() / 2;
        auto right = std::make_unique<Node>(false);
        right->children.assign(
            std::make_move_iterator(node.children.begin() + middle),
            std::make_move_iterator(node.children.end())
        );
        node.children.resize(middle);
        refresh(node);
        refresh(*right);
        return Split{std::move(right)};
    }

    static bool underfull(const Node& node) {
        return node.leaf
            ? node.keys.size() < MIN_LEAF_KEYS
            : node.children.size() < MIN_INTERNAL_CHILDREN;
    }

    static bool can_lend(const Node& node) {
        return node.leaf
            ? node.keys.size() > MIN_LEAF_KEYS
            : node.children.size() > MIN_INTERNAL_CHILDREN;
    }

    static void borrow_from_left(Node& parent, std::size_t index) {
        Node& left = *parent.children[index - 1];
        Node& child = *parent.children[index];
        if (child.leaf) {
            child.keys.insert(child.keys.begin(), std::move(left.keys.back()));
            left.keys.pop_back();
            child.values.insert(
                child.values.begin(), std::move(left.values.back())
            );
            left.values.pop_back();
        } else {
            child.children.insert(
                child.children.begin(), std::move(left.children.back())
            );
            left.children.pop_back();
        }
        refresh(left);
        refresh(child);
    }

    static void borrow_from_right(Node& parent, std::size_t index) {
        Node& child = *parent.children[index];
        Node& right = *parent.children[index + 1];
        if (child.leaf) {
            child.keys.push_back(std::move(right.keys.front()));
            right.keys.erase(right.keys.begin());
            child.values.push_back(std::move(right.values.front()));
            right.values.erase(right.values.begin());
        } else {
            child.children.push_back(std::move(right.children.front()));
            right.children.erase(right.children.begin());
        }
        refresh(child);
        refresh(right);
    }

    static void merge_right_into_left(Node& left, Node& right) {
        if (left.leaf) {
            left.keys.insert(
                left.keys.end(),
                std::make_move_iterator(right.keys.begin()),
                std::make_move_iterator(right.keys.end())
            );
            left.values.insert(
                left.values.end(),
                std::make_move_iterator(right.values.begin()),
                std::make_move_iterator(right.values.end())
            );
            left.next = right.next;
        } else {
            left.children.insert(
                left.children.end(),
                std::make_move_iterator(right.children.begin()),
                std::make_move_iterator(right.children.end())
            );
        }
        refresh(left);
    }

    static void rebalance_child(Node& parent, std::size_t index) {
        if (!underfull(*parent.children[index])) return;
        if (index > 0 && can_lend(*parent.children[index - 1])) {
            borrow_from_left(parent, index);
            return;
        }
        if (index + 1 < parent.children.size()
            && can_lend(*parent.children[index + 1])) {
            borrow_from_right(parent, index);
            return;
        }
        if (index > 0) {
            merge_right_into_left(
                *parent.children[index - 1], *parent.children[index]
            );
            parent.children.erase(
                parent.children.begin() + static_cast<std::ptrdiff_t>(index)
            );
        } else {
            merge_right_into_left(
                *parent.children[0], *parent.children[1]
            );
            parent.children.erase(parent.children.begin() + 1);
        }
    }

    bool erase_impl(Node& node, const Key& key) {
        if (node.leaf) {
            const std::size_t index = lower_position(node.keys, key);
            if (index == node.keys.size() || !equal(node.keys[index], key)) {
                return false;
            }
            node.keys.erase(
                node.keys.begin() + static_cast<std::ptrdiff_t>(index)
            );
            node.values.erase(
                node.values.begin() + static_cast<std::ptrdiff_t>(index)
            );
            refresh(node);
            return true;
        }

        std::size_t index = child_position(node.keys, key);
        if (!erase_impl(*node.children[index], key)) return false;
        rebalance_child(node, index);
        refresh(node);
        return true;
    }

    Node* leaf_for(const Key& key) {
        Node* node = root_.get();
        while (!node->leaf) {
            node = node->children[child_position(node->keys, key)].get();
        }
        return node;
    }

    const Node* leaf_for(const Key& key) const {
        const Node* node = root_.get();
        while (!node->leaf) {
            node = node->children[child_position(node->keys, key)].get();
        }
        return node;
    }

public:
    StandardBPlusTree() = default;
    explicit StandardBPlusTree(Compare compare)
        : compare_(std::move(compare)) {}
    StandardBPlusTree(StandardBPlusTree&&) noexcept = default;
    StandardBPlusTree& operator=(StandardBPlusTree&&) noexcept = default;
    StandardBPlusTree(const StandardBPlusTree&) = delete;
    StandardBPlusTree& operator=(const StandardBPlusTree&) = delete;

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    bool insert_or_assign(const Key& key, const Value& value) {
        bool inserted = false;
        auto split = insert_impl(*root_, key, value, inserted);
        if (split) {
            auto new_root = std::make_unique<Node>(false);
            new_root->children.push_back(std::move(root_));
            new_root->children.push_back(std::move(split->right));
            refresh(*new_root);
            root_ = std::move(new_root);
        }
        if (inserted) ++size_;
        return inserted;
    }

    [[nodiscard]] std::optional<Value> find(const Key& key) const {
        const Node* leaf = leaf_for(key);
        const std::size_t index = lower_position(leaf->keys, key);
        if (index == leaf->keys.size() || !equal(leaf->keys[index], key)) {
            return std::nullopt;
        }
        return leaf->values[index];
    }

    bool erase(const Key& key) {
        if (!erase_impl(*root_, key)) return false;
        --size_;
        while (!root_->leaf && root_->children.size() == 1) {
            root_ = std::move(root_->children.front());
        }
        refresh(*root_);
        return true;
    }

    [[nodiscard]] std::vector<std::pair<Key, Value>> range(
        const Key& lower, const Key& upper
    ) const {
        std::vector<std::pair<Key, Value>> result;
        const Node* leaf = leaf_for(lower);
        std::size_t index = lower_position(leaf->keys, lower);
        while (leaf) {
            while (index < leaf->keys.size()) {
                if (!compare_(leaf->keys[index], upper)) return result;
                result.emplace_back(leaf->keys[index], leaf->values[index]);
                ++index;
            }
            leaf = leaf->next;
            index = 0;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_DETAIL_STANDARD_B_PLUS_TREE_HPP_INCLUDED
