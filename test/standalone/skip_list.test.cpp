// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/bbst/probabilistic/skip_list.hpp"
int main() {
    SkipList<int, int> list;
    for (int x = 0; x < 100; ++x) assert(list.insert_or_assign(x, x * x));
    for (int x = 0; x < 100; x += 3) assert(list.erase(x));
    assert(*list.find(10) == 100 && !list.find(12));
    assert(list.lower_bound(12)->first == 13);
    std::cout << "OK\n";
}
