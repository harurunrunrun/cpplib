// competitive-verifier: STANDALONE
#include <cassert>
#include <functional>
#include <iostream>
#include "../../src/structure/heap/priority_queue/deletable_heap.hpp"
int main() {
    DeletableHeap<int, std::greater<int>> heap;
    for (int x : {8, 2, 5, 1}) heap.push(x);
    heap.erase(1);
    assert(heap.top() == 2);
    heap.pop();
    assert(heap.top() == 5);
    std::cout << "OK\n";
}
