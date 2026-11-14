// competitive-verifier: STANDALONE
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <utility>
#include <vector>
#include "../../src/structure/heap/meldable/pairing_heap.hpp"
void check_long_chains() {
    constexpr int n = 1 << 20;
    {
        PairingHeap<int> ascending;
        for (int x = 0; x < n; ++x) ascending.push(x);
        assert(ascending.size() == static_cast<std::size_t>(n) && ascending.top() == 0);
    }
    {
        PairingHeap<int> descending;
        for (int x = n; x > 0; --x) descending.push(x);
        assert(descending.top() == 1);
        descending.clear();
        assert(descending.empty());
        for (int x = n; x > 0; --x) descending.push(x);
        PairingHeap<int> other;
        for (int x = 0; x < n; ++x) other.push(n + x);
        descending.meld(std::move(other));
        assert(descending.size() == static_cast<std::size_t>(2 * n) && descending.top() == 1);
        PairingHeap<int> moved_to;
        for (int x = 0; x < n; ++x) moved_to.push(x);
        moved_to = std::move(descending);
        assert(moved_to.top() == 1);
    }
}

void check_random_against_priority_queue() {
    std::mt19937 random(20261114);
    PairingHeap<int> heap, other;
    std::priority_queue<int, std::vector<int>, std::greater<int>> expected;
    for (int step = 0; step < 200000; ++step) {
        const unsigned operation = random() % 8;
        if (operation < 4) {
            const int value = static_cast<int>(random() % 1000);
            heap.push(value);
            expected.push(value);
        } else if (operation < 6) {
            heap.meld(std::move(other));
            other = PairingHeap<int>();
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
            other = PairingHeap<int>();
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
    PairingHeap<int> a, b;
    for (int x : {7, 2, 9}) a.push(x);
    for (int x : {5, 1}) b.push(x);
    a.meld(std::move(b));
    for (int x : {1, 2, 5, 7, 9}) { assert(a.top() == x); a.pop(); }
    std::cout << "OK\n";
}
