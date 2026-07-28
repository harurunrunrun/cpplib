// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/bbst/multiway/b_tree.hpp"
int main() {
    BTree<int, 4> tree;
    for (int x = 0; x < 1000; ++x) assert(tree.insert(x));
    for (int x = 0; x < 1000; x += 2) assert(tree.erase(x));
    assert(tree.size() == 500 && tree.lower_bound(500) == 501);
    std::cout << "OK\n";
}
