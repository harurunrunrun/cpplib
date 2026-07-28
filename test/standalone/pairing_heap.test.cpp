// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/heap/meldable/pairing_heap.hpp"
int main() {
    PairingHeap<int> a, b;
    for (int x : {7, 2, 9}) a.push(x);
    for (int x : {5, 1}) b.push(x);
    a.meld(std::move(b));
    for (int x : {1, 2, 5, 7, 9}) { assert(a.top() == x); a.pop(); }
    std::cout << "OK\n";
}
