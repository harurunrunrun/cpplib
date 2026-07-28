#ifndef CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_BINOMIAL_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_BINOMIAL_HEAP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Compare = std::less<T>>
class BinomialHeap {
    struct Node {
        T value;
        std::unique_ptr<Node> child;
        std::unique_ptr<Node> sibling;
        explicit Node(T value_) : value(std::move(value_)) {}
    };
    std::vector<std::unique_ptr<Node>> roots_;
    std::size_t size_ = 0;
    Compare compare_;

    std::unique_ptr<Node> link(
        std::unique_ptr<Node> a, std::unique_ptr<Node> b
    ) {
        if (compare_(b->value, a->value)) std::swap(a, b);
        b->sibling = std::move(a->child);
        a->child = std::move(b);
        return a;
    }

    void add_tree(std::unique_ptr<Node> tree, std::size_t order) {
        if (roots_.size() <= order) roots_.resize(order + 1);
        while (roots_[order]) {
            tree = link(std::move(tree), std::move(roots_[order]));
            ++order;
            if (roots_.size() <= order) roots_.resize(order + 1);
        }
        roots_[order] = std::move(tree);
    }

public:
    BinomialHeap() = default;
    explicit BinomialHeap(Compare compare) : compare_(std::move(compare)) {}
    BinomialHeap(BinomialHeap&&) noexcept = default;
    BinomialHeap& operator=(BinomialHeap&&) noexcept = default;
    BinomialHeap(const BinomialHeap&) = delete;
    BinomialHeap& operator=(const BinomialHeap&) = delete;

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    const T& top() const {
        const Node* best = nullptr;
        for (const auto& root : roots_) {
            if (root && (!best || compare_(root->value, best->value))) {
                best = root.get();
            }
        }
        if (!best) throw std::out_of_range("BinomialHeap::top on empty heap");
        return best->value;
    }
    void push(const T& value) {
        add_tree(std::make_unique<Node>(value), 0);
        ++size_;
    }
    void push(T&& value) {
        add_tree(std::make_unique<Node>(std::move(value)), 0);
        ++size_;
    }
    void meld(BinomialHeap&& other) {
        const std::size_t old_size = size_;
        auto old_roots = std::move(roots_);
        roots_.clear();
        for (std::size_t order = 0; order < old_roots.size(); ++order) {
            if (old_roots[order]) {
                add_tree(std::move(old_roots[order]), order);
            }
        }
        for (std::size_t order = 0; order < other.roots_.size(); ++order) {
            if (other.roots_[order]) {
                add_tree(std::move(other.roots_[order]), order);
            }
        }
        size_ = old_size + other.size_;
        other.size_ = 0;
        other.roots_.clear();
    }
    void pop() {
        if (empty()) throw std::out_of_range("BinomialHeap::pop on empty heap");
        std::size_t best = roots_.size();
        for (std::size_t i = 0; i < roots_.size(); ++i) {
            if (roots_[i] && (best == roots_.size()
                || compare_(roots_[i]->value, roots_[best]->value))) best = i;
        }
        auto children = std::move(roots_[best]->child);
        roots_[best].reset();
        std::size_t order = best;
        while (children) {
            auto next = std::move(children->sibling);
            children->sibling.reset();
            --order;
            add_tree(std::move(children), order);
            children = std::move(next);
        }
        --size_;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_MELDABLE_BINOMIAL_HEAP_HPP_INCLUDED
