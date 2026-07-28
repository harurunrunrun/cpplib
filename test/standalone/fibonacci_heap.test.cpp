// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/heap/meldable/fibonacci_heap.hpp"
int main() {
    FibonacciHeap<int> heap;
    auto handle = heap.push(8);
    heap.push(3);
    heap.decrease_key(handle, 1);
    assert(heap.top() == 1);
    heap.pop();
    assert(heap.top() == 3 && !handle.valid());
    heap.pop();
    for (int value = 999; value >= 0; --value) heap.push(value);
    for (int value = 0; value < 1000; ++value) {
        assert(heap.top() == value);
        heap.pop();
    }
    assert(heap.empty());
    std::cout << "OK\n";
}
