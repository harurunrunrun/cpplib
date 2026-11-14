// competitive-verifier: STANDALONE
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <utility>
#include <vector>
#include "../../src/structure/heap/meldable/skew_heap.hpp"
void check_long_chains() {
    constexpr int n = 1 << 20;
    {
        SkewHeap<int> ascending;
        for (int x = 0; x < n; ++x) ascending.push(x);
        assert(ascending.size() == static_cast<std::size_t>(n) && ascending.top() == 0);
    }
    {
        SkewHeap<int> descending;
        for (int x = n; x > 0; --x) descending.push(x);
        assert(descending.top() == 1);
        SkewHeap<int> other;
        for (int x = 0; x < n; ++x) other.push(n + x);
        descending.meld(std::move(other));
        assert(descending.size() == static_cast<std::size_t>(2 * n) && descending.top() == 1);
        SkewHeap<int> moved_to;
        for (int x = 0; x < n; ++x) moved_to.push(x);
        moved_to = std::move(descending);
        assert(moved_to.top() == 1);
    }
}

void check_random_against_priority_queue() {
    std::mt19937 random(20261114);
    SkewHeap<int> heap, other;
    std::priority_queue<int, std::vector<int>, std::greater<int>> expected;
    for (int step = 0; step < 200000; ++step) {
        const unsigned operation = random() % 8;
        if (operation < 4) {
            const int value = static_cast<int>(random() % 1000);
            heap.push(value);
            expected.push(value);
        } else if (operation < 6) {
            heap.meld(std::move(other));
            other = SkewHeap<int>();
            if (!expected.empty()) {
                assert(heap.top() == expected.top());
                heap.pop();
                expected.pop();
            }
        } else if (operation == 6) {
            const int value = static_cast<int>(random() % 1000);
            other.push(value);
            expected.push(value);
        } else {
            heap.meld(std::move(other));
            other = SkewHeap<int>();
        }
        assert(heap.size() + other.size() == expected.size());
    }
    heap.meld(std::move(other));
    while (!expected.empty()) {
        assert(heap.top() == expected.top());
        heap.pop();
        expected.pop();
    }
}

int main() {
    check_long_chains();
    check_random_against_priority_queue();
    SkewHeap<int> heap;
    for (int x : {4, 8, 1, 6, 3}) heap.push(x);
    for (int x : {1, 3, 4, 6, 8}) { assert(heap.top() == x); heap.pop(); }
    std::cout << "OK\n";
}
