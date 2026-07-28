// competitive-verifier: STANDALONE
#include <cassert>
#include <cstdint>
#include <iostream>
#include "../../src/structure/ordered_set/integer/x_fast_trie.hpp"
int main() {
    XFastTrie<std::uint64_t, 16> tree;
    for (auto x : {1U, 9U, 300U, 65535U}) assert(tree.insert(x));
    assert(tree.lower_bound(10) == 300 && tree.predecessor(300) == 9);
    assert(tree.erase(9) && !tree.contains(9));
    std::cout << "OK\n";
}
