// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/hash/robin_hood_hash_table.hpp"
int main() {
    RobinHoodHashTable<int, int> table;
    for (int x = 0; x < 1000; ++x) assert(table.insert_or_assign(x, x * x));
    for (int x = 0; x < 1000; x += 3) assert(table.erase(x));
    assert(table.at(10) == 100 && !table.contains(12));
    std::cout << "OK\n";
}
