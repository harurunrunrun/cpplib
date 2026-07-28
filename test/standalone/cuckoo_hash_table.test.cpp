// competitive-verifier: STANDALONE
#include <cassert>
#include <cstddef>
#include <iostream>
#include "../../src/structure/hash/cuckoo_hash_table.hpp"

struct ConstantHash {
    std::size_t operator()(int) const noexcept { return 0; }
};

int main() {
    CuckooHashTable<int, int> table;
    for (int x = 0; x < 1000; ++x) assert(table.insert_or_assign(x, x * x));
    for (int x = 0; x < 1000; x += 3) assert(table.erase(x));
    assert(table.at(10) == 100 && !table.contains(12));

    CuckooHashTable<int, int, ConstantHash> collisions;
    for (int x = 0; x < 200; ++x) {
        assert(collisions.insert_or_assign(x, x * x));
    }
    assert(collisions.size() == 200);
    for (int x = 0; x < 200; ++x) assert(collisions.at(x) == x * x);
    for (int x = 0; x < 200; x += 2) {
        assert(!collisions.insert_or_assign(x, -x));
    }
    for (int x = 0; x < 200; ++x) {
        assert(collisions.at(x) == (x % 2 == 0 ? -x : x * x));
    }
    for (int x = 0; x < 200; x += 3) assert(collisions.erase(x));
    for (int x = 0; x < 200; ++x) {
        assert(collisions.contains(x) == (x % 3 != 0));
    }
    assert(!collisions.contains(1000));

    std::cout << "OK\n";
}
