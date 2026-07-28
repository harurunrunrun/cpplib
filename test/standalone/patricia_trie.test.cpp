// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/trie/string/patricia_trie.hpp"
int main() {
    PatriciaTrie trie;
    for (const char* s : {"", "abc", "abd", "a", "xyz"}) assert(trie.insert(s));
    assert(trie.contains("abc") && !trie.contains("ab"));
    assert(trie.erase("abc") && trie.contains("abd"));
    std::cout << "OK\n";
}
