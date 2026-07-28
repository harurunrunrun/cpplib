// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/hash/perfect_hash_set.hpp"

struct ConstantHash {
    std::size_t operator()(int) const noexcept { return 0; }
};

struct SeparatedHighHash {
    std::size_t operator()(int value) const noexcept {
        return static_cast<std::size_t>(
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(value))
            << 32
        );
    }
};

int main() {
    std::vector<int> keys;
    for (int x = 0; x < 1000; ++x) keys.push_back(7 * x);
    PerfectHashSet<int> set(keys);
    for (int x = 0; x < 7000; ++x) {
        assert(set.contains(x) == (x % 7 == 0));
    }

    PerfectHashSet<int, ConstantHash> one({7, 7, 7, 7});
    assert(one.size() == 1);
    assert(one.contains(7));
    assert(!one.contains(8));

    bool rejected = false;
    try {
        one.build({1, 1, 2});
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
    assert(one.size() == 1 && one.contains(7));

    std::mt19937 random(0x41537U);
    PerfectHashSet<int, SeparatedHighHash> rebuilt;
    for (int repetition = 0; repetition < 40; ++repetition) {
        std::vector<int> values;
        const int count = 1500 + static_cast<int>(random() % 1000);
        values.reserve(static_cast<std::size_t>(count) * 2);
        for (int i = 0; i < count; ++i) {
            const int value = repetition * 100000 + i * 3;
            values.push_back(value);
            if (random() % 3 == 0) values.push_back(value);
        }
        rebuilt.build(values);
        assert(rebuilt.size() == static_cast<std::size_t>(count));
        for (int i = -10; i < count * 3 + 10; ++i) {
            const int value = repetition * 100000 + i;
            assert(rebuilt.contains(value)
                == (0 <= i && i < count * 3 && i % 3 == 0));
        }
    }

    rebuilt.build({});
    assert(rebuilt.empty());
    assert(!rebuilt.contains(0));

    std::cout << "OK\n";
}
