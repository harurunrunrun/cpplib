// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/ordered_set/integer/van_emde_boas_tree.hpp"
int main() {
    VanEmdeBoasTree<16> tree;
    for (auto x : {1U, 9U, 300U, 65535U}) assert(tree.insert(x));
    assert(tree.minimum() == 1 && tree.maximum() == 65535);
    assert(tree.successor(9) == 300 && tree.predecessor(300) == 9);
    assert(tree.erase(9) && !tree.contains(9));
    std::cout << "OK\n";
}
