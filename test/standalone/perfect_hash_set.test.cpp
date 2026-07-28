// competitive-verifier: STANDALONE
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>
#include "../../src/structure/hash/perfect_hash_set.hpp"
struct ConstantHash {
    std::size_t operator()(int) const noexcept { return 0; }
};

int main() {
    std::vector<int> keys;
    for (int x = 0; x < 1000; ++x) keys.push_back(7 * x);
    PerfectHashSet<int> set(keys);
    for (int x = 0; x < 7000; ++x) assert(set.contains(x) == (x % 7 == 0));
    std::vector<int> colliding_keys;
    for (int x = 0; x < 200; ++x) colliding_keys.push_back(x);
    PerfectHashSet<int, ConstantHash> colliding(colliding_keys);
    assert(colliding.size() == 200);
    for (int x = -20; x < 250; ++x) {
        assert(colliding.contains(x) == (0 <= x && x < 200));
    }

    std::cout << "OK\n";
}
