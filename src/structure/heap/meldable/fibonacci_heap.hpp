#ifndef CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_FIBONACCI_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_FIBONACCI_HEAP_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Compare = std::less<T>>
class FibonacciHeap {
    struct Node {
        T value;
        Node* parent = nullptr;
        Node* child = nullptr;
        Node* left = this;
        Node* right = this;
        std::size_t degree = 0;
        bool mark = false;
        bool active = true;
        explicit Node(T value_) : value(std::move(value_)) {}
    };

public:
    class Handle {
        Node* node_ = nullptr;
        explicit Handle(Node* node) : node_(node) {}
        friend class FibonacciHeap;
    public:
        Handle() = default;
        [[nodiscard]] bool valid() const noexcept {
            return node_ != nullptr && node_->active;
        }
    };

private:
    Node* minimum_ = nullptr;
    std::size_t size_ = 0;
    Compare compare_;
    std::vector<std::unique_ptr<Node>> storage_;

    static void remove_from_list(Node* node) {
        node->left->right = node->right;
        node->right->left = node->left;
        node->left = node;
        node->right = node;
    }

    void insert_root(Node* node) {
        node->parent = nullptr;
        node->mark = false;
        if (!minimum_) {
            minimum_ = node;
            node->left = node;
            node->right = node;
            return;
        }
        node->right = minimum_->right;
        node->left = minimum_;
        minimum_->right->left = node;
        minimum_->right = node;
        if (compare_(node->value, minimum_->value)) minimum_ = node;
    }

    void make_child(Node* child, Node* parent) {
        remove_from_list(child);
        child->parent = parent;
        child->mark = false;
        if (!parent->child) {
            parent->child = child;
        } else {
            child->right = parent->child->right;
            child->left = parent->child;
            parent->child->right->left = child;
            parent->child->right = child;
        }
        ++parent->degree;
    }

    std::vector<Node*> list_nodes(Node* start) const {
        std::vector<Node*> result;
        if (!start) return result;
        Node* current = start;
        do {
            result.push_back(current);
            current = current->right;
        } while (current != start);
        return result;
    }

    void consolidate() {
        const std::size_t limit =
            2 * static_cast<std::size_t>(std::bit_width(size_)) + 2;
        std::vector<Node*> by_degree(limit, nullptr);
        auto roots = list_nodes(minimum_);
        for (Node* node : roots) {
            if (!node->active || node->parent) continue;
            std::size_t degree = node->degree;
            while (by_degree[degree]) {
                Node* other = by_degree[degree];
                if (compare_(other->value, node->value)) std::swap(node, other);
                make_child(other, node);
                by_degree[degree] = nullptr;
                ++degree;
            }
            by_degree[degree] = node;
        }
        minimum_ = nullptr;
        for (Node* node : by_degree) {
            if (!node) continue;
            remove_from_list(node);
            insert_root(node);
        }
    }

    void cut(Node* node, Node* parent) {
        if (parent->child == node) {
            parent->child = node->right == node ? nullptr : node->right;
        }
        remove_from_list(node);
        --parent->degree;
        insert_root(node);
    }

    void cascading_cut(Node* node) {
        Node* parent = node->parent;
        if (!parent) return;
        if (!node->mark) {
            node->mark = true;
            return;
        }
        cut(node, parent);
        cascading_cut(parent);
    }

public:
    FibonacciHeap() = default;
    explicit FibonacciHeap(Compare compare) : compare_(std::move(compare)) {}
    FibonacciHeap(FibonacciHeap&&) noexcept = default;
    FibonacciHeap& operator=(FibonacciHeap&&) noexcept = default;
    FibonacciHeap(const FibonacciHeap&) = delete;
    FibonacciHeap& operator=(const FibonacciHeap&) = delete;

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    const T& top() const {
        if (!minimum_) throw std::out_of_range("FibonacciHeap::top");
        return minimum_->value;
    }

    Handle push(const T& value) { return emplace(value); }
    Handle push(T&& value) { return emplace(std::move(value)); }
    template<class... Args>
    Handle emplace(Args&&... args) {
        auto holder = std::make_unique<Node>(
            T(std::forward<Args>(args)...)
        );
        Node* node = holder.get();
        storage_.push_back(std::move(holder));
        insert_root(node);
        ++size_;
        return Handle(node);
    }

    void decrease_key(Handle handle, T value) {
        Node* node = handle.node_;
        if (!node || !node->active) {
            throw std::invalid_argument("FibonacciHeap invalid handle");
        }
        if (compare_(node->value, value)) {
            throw std::invalid_argument("FibonacciHeap key increased");
        }
        node->value = std::move(value);
        Node* parent = node->parent;
        if (parent && compare_(node->value, parent->value)) {
            cut(node, parent);
            cascading_cut(parent);
        }
        if (compare_(node->value, minimum_->value)) minimum_ = node;
    }

    void pop() {
        if (!minimum_) throw std::out_of_range("FibonacciHeap::pop");
        Node* removed = minimum_;
        auto children = list_nodes(removed->child);
        for (Node* child : children) {
            remove_from_list(child);
            insert_root(child);
        }
        removed->child = nullptr;
        if (removed->right == removed) {
            minimum_ = nullptr;
        } else {
            Node* next = removed->right;
            remove_from_list(removed);
            minimum_ = next;
        }
        removed->active = false;
        --size_;
        if (minimum_) consolidate();
    }

    void meld(FibonacciHeap&& other) {
        if (!other.minimum_) return;
        if (!minimum_) {
            minimum_ = other.minimum_;
        } else {
            Node* a_right = minimum_->right;
            Node* b_left = other.minimum_->left;
            minimum_->right = other.minimum_;
            other.minimum_->left = minimum_;
            a_right->left = b_left;
            b_left->right = a_right;
            if (compare_(other.minimum_->value, minimum_->value)) {
                minimum_ = other.minimum_;
            }
        }
        size_ += other.size_;
        for (auto& holder : other.storage_) {
            storage_.push_back(std::move(holder));
        }
        other.storage_.clear();
        other.minimum_ = nullptr;
        other.size_ = 0;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_FIBONACCI_HEAP_HPP_INCLUDED
