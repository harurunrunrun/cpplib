#ifndef CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_PAIRING_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_PAIRING_HEAP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Compare = std::less<T>>
class PairingHeap {
    struct Node {
        T value;
        std::unique_ptr<Node> child;
        std::unique_ptr<Node> sibling;
        explicit Node(T value_) : value(std::move(value_)) {}
    };

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
    Compare compare_;

    std::unique_ptr<Node> meld_nodes(
        std::unique_ptr<Node> first, std::unique_ptr<Node> second
    ) {
        if (!first) return second;
        if (!second) return first;
        if (compare_(second->value, first->value)) std::swap(first, second);
        second->sibling = std::move(first->child);
        first->child = std::move(second);
        return first;
    }

    std::unique_ptr<Node> merge_pairs(std::unique_ptr<Node> node) {
        if (!node || !node->sibling) return node;
        std::vector<std::unique_ptr<Node>> trees;
        while (node) {
            auto first = std::move(node);
            node = std::move(first->sibling);
            first->sibling.reset();
            if (node) {
                auto second = std::move(node);
                node = std::move(second->sibling);
                second->sibling.reset();
                trees.push_back(meld_nodes(std::move(first), std::move(second)));
            } else {
                trees.push_back(std::move(first));
            }
        }
        auto result = std::move(trees.back());
        trees.pop_back();
        while (!trees.empty()) {
            result = meld_nodes(std::move(trees.back()), std::move(result));
            trees.pop_back();
        }
        return result;
    }

public:
    PairingHeap() = default;
    explicit PairingHeap(Compare compare) : compare_(std::move(compare)) {}
    PairingHeap(PairingHeap&&) noexcept = default;
    PairingHeap& operator=(PairingHeap&&) noexcept = default;
    PairingHeap(const PairingHeap&) = delete;
    PairingHeap& operator=(const PairingHeap&) = delete;

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    const T& top() const {
        if (!root_) throw std::out_of_range("PairingHeap::top on empty heap");
        return root_->value;
    }

    void push(const T& value) {
        root_ = meld_nodes(std::move(root_), std::make_unique<Node>(value));
        ++size_;
    }
    void push(T&& value) {
        root_ = meld_nodes(
            std::move(root_), std::make_unique<Node>(std::move(value))
        );
        ++size_;
    }

    void pop() {
        if (!root_) throw std::out_of_range("PairingHeap::pop on empty heap");
        root_ = merge_pairs(std::move(root_->child));
        --size_;
    }

    void meld(PairingHeap&& other) {
        root_ = meld_nodes(std::move(root_), std::move(other.root_));
        size_ += other.size_;
        other.size_ = 0;
    }

    void clear() noexcept {
        root_.reset();
        size_ = 0;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_PAIRING_HEAP_HPP_INCLUDED
