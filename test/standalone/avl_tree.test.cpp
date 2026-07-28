// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/bbst/set/avl_tree.hpp"
int main() {
    AvlTree<int> tree;
    for (int x = 100; x >= 0; --x) assert(tree.insert(x));
    for (int x = 0; x <= 100; x += 2) assert(tree.erase(x));
    assert(tree.lower_bound(50) == 51 && tree.kth(0) == 1);
    std::cout << "OK\n";
}
