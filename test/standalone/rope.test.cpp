// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/string/rope.hpp"
int main() {
    Rope rope("abcdef");
    rope.insert(3, "XYZ");
    rope.erase(1, 2);
    rope.set(0, 'Q');
    assert(rope.str() == "QXYZdef" && rope.substr(1, 3) == "XYZ");
    std::cout << "OK\n";
}
