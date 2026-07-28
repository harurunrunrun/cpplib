#ifndef CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_SKEW_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_SKEW_HEAP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

template<class T, class Compare = std::less<T>>
class SkewHeap {
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(T value_) : value(std::move(value_)) {}
    };
    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
    Compare compare_;

    std::unique_ptr<Node> meld_nodes(
        std::unique_ptr<Node> a, std::unique_ptr<Node> b
    ) {
        if (!a) return b;
        if (!b) return a;
        if (compare_(b->value, a->value)) std::swap(a, b);
        a->right = meld_nodes(std::move(a->right), std::move(b));
        std::swap(a->left, a->right);
        return a;
    }

public:
    SkewHeap() = default;
    explicit SkewHeap(Compare compare) : compare_(std::move(compare)) {}
    SkewHeap(SkewHeap&&) noexcept = default;
    SkewHeap& operator=(SkewHeap&&) noexcept = default;
    SkewHeap(const SkewHeap&) = delete;
    SkewHeap& operator=(const SkewHeap&) = delete;

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    const T& top() const {
        if (!root_) throw std::out_of_range("SkewHeap::top on empty heap");
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
        if (!root_) throw std::out_of_range("SkewHeap::pop on empty heap");
        root_ = meld_nodes(std::move(root_->left), std::move(root_->right));
        --size_;
    }
    void meld(SkewHeap&& other) {
        root_ = meld_nodes(std::move(root_), std::move(other.root_));
        size_ += other.size_;
        other.size_ = 0;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_SKEW_HEAP_HPP_INCLUDED
