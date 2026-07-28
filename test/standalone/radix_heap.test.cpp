// competitive-verifier: STANDALONE
#include <cassert>
#include <cstdint>
#include <iostream>
#include "../../src/structure/heap/priority_queue/radix_heap.hpp"
int main() {
    RadixHeap<std::uint64_t, int> heap;
    for (std::uint64_t x : {0, 9, 2, 2, 100}) heap.push(x, int(x));
    std::uint64_t last = 0;
    while (!heap.empty()) { auto x = heap.pop(); assert(last <= x.first); last = x.first; }
    std::cout << "OK\n";
}
