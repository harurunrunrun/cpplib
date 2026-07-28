// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <utility>
#include <vector>

#include "../../src/structure/bbst/multiway/detail/standard_b_plus_tree.hpp"
#include "../../src/structure/bbst/multiway/b_plus_tree.hpp"

int main() {
    BPlusTree<int, int, 4> tree;
    std::map<int, int> expected;
    std::mt19937 random(20260729);

    for (int step = 0; step < 30000; ++step) {
        const int key = static_cast<int>(random() % 400);
        const int operation = static_cast<int>(random() % 5);
        if (operation <= 1) {
            const int value = static_cast<int>(random());
            const bool inserted = !expected.contains(key);
            expected[key] = value;
            assert(tree.insert_or_assign(key, value) == inserted);
        } else if (operation == 2) {
            assert(tree.erase(key) == (expected.erase(key) != 0));
        } else if (operation == 3) {
            const auto value = tree.find(key);
            const auto iterator = expected.find(key);
            assert(value.has_value() == (iterator != expected.end()));
            if (value) assert(*value == iterator->second);
        } else {
            int other = static_cast<int>(random() % 400);
            const int lower = std::min(key, other);
            const int upper = std::max(key, other);
            std::vector<std::pair<int, int>> wanted;
            for (auto iterator = expected.lower_bound(lower);
                 iterator != expected.end() && iterator->first < upper;
                 ++iterator) {
                wanted.push_back(*iterator);
            }
            assert(tree.range(lower, upper) == wanted);
        }
        assert(tree.size() == expected.size());
        assert(tree.empty() == expected.empty());
    }

    for (int round = 0; round < 4; ++round) {
        for (int key = 0; key < 2000; ++key) {
            const bool inserted = !expected.contains(key);
            expected[key] = key + round;
            assert(tree.insert_or_assign(key, key + round) == inserted);
        }
        if (round % 2 == 0) {
            for (int key = 0; key < 2000; ++key) {
                assert(tree.erase(key));
                expected.erase(key);
            }
        } else {
            for (int key = 1999; key >= 0; --key) {
                assert(tree.erase(key));
                expected.erase(key);
            }
        }
        assert(tree.size() == expected.size());
    }

    for (const auto& [key, value] : expected) {
        assert(tree.find(key) == value);
    }
    std::cout << "OK\n";
}
