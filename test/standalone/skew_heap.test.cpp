// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/heap/meldable/skew_heap.hpp"
int main() {
    SkewHeap<int> heap;
    for (int x : {4, 8, 1, 6, 3}) heap.push(x);
    for (int x : {1, 3, 4, 6, 8}) { assert(heap.top() == x); heap.pop(); }
    std::cout << "OK\n";
}
