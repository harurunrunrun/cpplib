// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/ordered_set/integer/detail/y_fast_trie_implementation.hpp"
#include "../../src/structure/ordered_set/integer/y_fast_trie.hpp"
int main() {
    YFastTrie<16> tree;
    for (auto x : {1U, 9U, 300U, 65535U}) assert(tree.insert(x));
    assert(tree.successor(9) == 300 && tree.predecessor(300) == 9);
    assert(tree.erase(9) && !tree.contains(9));
    std::cout << "OK\n";
}
