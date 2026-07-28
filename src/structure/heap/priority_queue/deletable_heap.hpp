#ifndef CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DELETABLE_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DELETABLE_HEAP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Compare = std::less<T>>
class DeletableHeap {
    using Queue = std::priority_queue<T, std::vector<T>, Compare>;
    Compare compare_;
    Queue inserted_;
    Queue erased_;
    std::size_t size_ = 0;
    DeletableHeap(const DeletableHeap&) = delete;
    DeletableHeap& operator=(const DeletableHeap&) = delete;

    void normalize() {
        while (!erased_.empty() && !inserted_.empty()
               && !compare_(inserted_.top(), erased_.top())
               && !compare_(erased_.top(), inserted_.top())) {
            inserted_.pop();
            erased_.pop();
        }
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    void push(const T& value) { inserted_.push(value); ++size_; }
    void push(T&& value) { inserted_.push(std::move(value)); ++size_; }
    DeletableHeap()
        : inserted_(compare_), erased_(compare_) {}
    explicit DeletableHeap(Compare compare)
        : compare_(std::move(compare)), inserted_(compare_), erased_(compare_) {}

    void erase(const T& value) {
        if (size_ == 0) throw std::out_of_range("DeletableHeap::erase");
        erased_.push(value);
        --size_;
        normalize();
    }
    const T& top() {
        normalize();
        if (inserted_.empty()) {
            throw std::out_of_range("DeletableHeap::top on empty heap");
        }
        return inserted_.top();
    }
    void pop() {
        normalize();
        if (inserted_.empty()) {
            throw std::out_of_range("DeletableHeap::pop on empty heap");
        }
        inserted_.pop();
        --size_;
        normalize();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DELETABLE_HEAP_HPP_INCLUDED
