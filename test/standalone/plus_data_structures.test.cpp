// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "../../src/structure/bbst/multiway/b_plus_tree.hpp"
#include "../../src/structure/bbst/multiway/b_tree.hpp"
#include "../../src/structure/bbst/probabilistic/skip_list.hpp"
#include "../../src/structure/bbst/sequence/finger_tree.hpp"
#include "../../src/structure/bbst/set/avl_tree.hpp"
#include "../../src/structure/bbst/set/scapegoat_tree.hpp"
#include "../../src/structure/compressed/elias_fano.hpp"
#include "../../src/structure/hash/cuckoo_hash_table.hpp"
#include "../../src/structure/hash/perfect_hash_set.hpp"
#include "../../src/structure/hash/robin_hood_hash_table.hpp"
#include "../../src/structure/heap/meldable/binomial_heap.hpp"
#include "../../src/structure/heap/meldable/fibonacci_heap.hpp"
#include "../../src/structure/heap/meldable/pairing_heap.hpp"
#include "../../src/structure/heap/meldable/skew_heap.hpp"
#include "../../src/structure/heap/priority_queue/deletable_heap.hpp"
#include "../../src/structure/heap/priority_queue/radix_heap.hpp"
#include "../../src/structure/ordered_set/integer/van_emde_boas_tree.hpp"
#include "../../src/structure/ordered_set/integer/x_fast_trie.hpp"
#include "../../src/structure/ordered_set/integer/y_fast_trie.hpp"
#include "../../src/structure/spatial/dynamic_kd_tree.hpp"
#include "../../src/structure/string/rope.hpp"
#include "../../src/structure/trie/string/crit_bit_tree.hpp"
#include "../../src/structure/trie/string/patricia_trie.hpp"

struct SumMonoid {
    using S = long long;
    constexpr S op(S a, S b) const { return a + b; }
    constexpr S e() const { return 0; }
};
constexpr SumMonoid sum_monoid{};

template<class Heap>
void test_heap() {
    Heap heap;
    std::priority_queue<int, std::vector<int>, std::greater<int>> expected;
    std::mt19937 random(41);
    for (int step = 0; step < 3000; ++step) {
        if (expected.empty() || random() % 3 != 0) {
            const int value = static_cast<int>(random() % 10000);
            heap.push(value);
            expected.push(value);
        } else {
            assert(heap.top() == expected.top());
            heap.pop();
            expected.pop();
        }
        assert(heap.size() == expected.size());
        if (!expected.empty()) assert(heap.top() == expected.top());
    }
}

template<class Ordered>
void test_ordered_integer() {
    Ordered structure;
    std::set<std::uint64_t> expected;
    std::mt19937 random(73);
    for (int step = 0; step < 5000; ++step) {
        const std::uint64_t key = random() % 65536;
        if (random() % 2) {
            assert(structure.insert(key) == expected.insert(key).second);
        } else {
            assert(structure.erase(key) == (expected.erase(key) != 0));
        }
        assert(structure.contains(key) == expected.contains(key));
        if (!expected.empty()) {
            assert(structure.minimum() == *expected.begin());
            assert(structure.maximum() == *expected.rbegin());
        }
        auto successor = structure.successor(key);
        auto next = expected.upper_bound(key);
        assert(successor.has_value() == (next != expected.end()));
        if (successor) assert(*successor == *next);
        auto previous = expected.lower_bound(key);
        const bool has_previous = previous != expected.begin();
        auto predecessor = structure.predecessor(key);
        assert(predecessor.has_value() == has_previous);
        if (has_previous) {
            --previous;
            assert(*predecessor == *previous);
        }
    }
}

template<class Tree>
void test_ordered_tree() {
    Tree tree;
    std::set<int> expected;
    std::mt19937 random(91);
    for (int step = 0; step < 4000; ++step) {
        const int key = static_cast<int>(random() % 1000);
        if (random() % 2) {
            assert(tree.insert(key) == expected.insert(key).second);
        } else {
            assert(tree.erase(key) == (expected.erase(key) != 0));
        }
        assert(tree.contains(key) == expected.contains(key));
        const auto actual = tree.lower_bound(key);
        const auto iterator = expected.lower_bound(key);
        assert(actual.has_value() == (iterator != expected.end()));
        if (actual) assert(*actual == *iterator);
    }
}

int main() {
    test_heap<PairingHeap<int, std::less<int>>>();
    test_heap<SkewHeap<int, std::less<int>>>();
    test_heap<BinomialHeap<int, std::less<int>>>();

    FibonacciHeap<int> fibonacci;
    auto first = fibonacci.push(10);
    fibonacci.push(3);
    fibonacci.decrease_key(first, 1);
    assert(fibonacci.top() == 1);
    fibonacci.pop();
    assert(fibonacci.top() == 3);

    RadixHeap<std::uint64_t, int> radix;
    for (std::uint64_t key : {0, 1, 1, 7, 30, 100}) {
        radix.push(key, static_cast<int>(key));
    }
    std::uint64_t previous = 0;
    while (!radix.empty()) {
        auto entry = radix.pop();
        assert(previous <= entry.first);
        previous = entry.first;
    }
    DeletableHeap<int, std::greater<int>> deletable;
    for (int value : {4, 1, 9, 2}) deletable.push(value);
    deletable.erase(1);
    assert(deletable.top() == 2);

    test_ordered_integer<VanEmdeBoasTree<16>>();
    test_ordered_integer<XFastTrie<std::uint64_t, 16>>();
    test_ordered_integer<YFastTrie<16>>();

    PatriciaTrie patricia;
    CritBitTree crit_bit;
    std::set<std::string> words{
        "", "a", "alpha", "alpine", "beta", "bet", "z"
    };
    for (const auto& word : words) {
        assert(patricia.insert(word));
        assert(crit_bit.insert(word));
    }
    assert(!patricia.insert("alpha"));
    assert(!crit_bit.insert("alpha"));
    for (const auto& word : words) {
        assert(patricia.contains(word));
        assert(crit_bit.contains(word));
    }
    assert(patricia.erase("alpha"));
    assert(crit_bit.erase("alpha"));
    words.erase("alpha");
    assert(patricia.keys() == std::vector<std::string>(
        words.begin(), words.end()
    ));
    assert(crit_bit.keys() == std::vector<std::string>(
        words.begin(), words.end()
    ));

    test_ordered_tree<AvlTree<int>>();
    test_ordered_tree<ScapegoatTree<int>>();
    test_ordered_tree<BTree<int, 4>>();

    BPlusTree<int, int, 8> bplus;
    std::map<int, int> expected_map;
    SkipList<int, int> skip;
    RobinHoodHashTable<int, int> robin;
    CuckooHashTable<int, int> cuckoo;
    for (int key = 0; key < 1000; ++key) {
        const int value = key * key;
        assert(bplus.insert_or_assign(key, value));
        assert(skip.insert_or_assign(key, value));
        assert(robin.insert_or_assign(key, value));
        assert(cuckoo.insert_or_assign(key, value));
        expected_map[key] = value;
    }
    for (int key = 0; key < 1000; key += 3) {
        assert(bplus.erase(key));
        assert(skip.erase(key));
        assert(robin.erase(key));
        assert(cuckoo.erase(key));
        expected_map.erase(key);
    }
    for (int key = 0; key < 1000; ++key) {
        const auto bplus_value = bplus.find(key);
        const int* skip_value = skip.find(key);
        const int* robin_value = robin.find(key);
        const int* cuckoo_value = cuckoo.find(key);
        const bool exists = expected_map.contains(key);
        assert(bplus_value.has_value() == exists);
        assert((skip_value != nullptr) == exists);
        assert((robin_value != nullptr) == exists);
        assert((cuckoo_value != nullptr) == exists);
        if (exists) {
            assert(*bplus_value == expected_map[key]);
            assert(*skip_value == expected_map[key]);
            assert(*robin_value == expected_map[key]);
            assert(*cuckoo_value == expected_map[key]);
        }
    }
    const auto range = bplus.range(100, 200);
    std::vector<std::pair<int, int>> expected_range;
    for (auto iterator = expected_map.lower_bound(100);
         iterator != expected_map.end() && iterator->first < 200; ++iterator) {
        expected_range.push_back(*iterator);
    }
    assert(range == expected_range);

    std::vector<int> perfect_keys;
    for (int key = 0; key < 1000; ++key) perfect_keys.push_back(key * 7);
    PerfectHashSet<int> perfect(perfect_keys);
    for (int key = 0; key < 7000; ++key) {
        assert(perfect.contains(key) == (key % 7 == 0));
    }

    FingerTree<sum_monoid> finger;
    std::vector<long long> sequence;
    for (long long value = 0; value < 200; ++value) {
        if (value % 2) {
            finger.push_front(value);
            sequence.insert(sequence.begin(), value);
        } else {
            finger.push_back(value);
            sequence.push_back(value);
        }
    }
    assert(finger.size() == sequence.size());
    assert(finger.fold() == 199LL * 200 / 2);
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        assert(finger.at(i) == sequence[i]);
    }
    auto [left, right] = std::move(finger).split(77);
    left.concatenate(std::move(right));
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        assert(left.at(i) == sequence[i]);
    }

    Rope rope("abcdefgh");
    rope.insert(3, "XYZ");
    rope.erase(1, 2);
    rope.set(0, 'Q');
    assert(rope.str() == "QXYZdefgh");
    assert(rope.substr(1, 3) == "XYZ");

    std::vector<std::uint64_t> monotone;
    for (std::uint64_t i = 0; i < 2000; ++i) {
        monotone.push_back(i * i / 3);
    }
    EliasFano elias(monotone);
    for (std::size_t i = 0; i < monotone.size(); ++i) {
        assert(elias.select(i) == monotone[i]);
    }
    for (std::uint64_t value = 0; value < 10000; ++value) {
        assert(elias.lower_bound(value) == static_cast<std::size_t>(
            std::lower_bound(monotone.begin(), monotone.end(), value)
            - monotone.begin()
        ));
    }

    DynamicKdTree<int, 2> kd;
    std::map<int, std::array<int, 2>> points;
    std::mt19937 random(119);
    for (int id = 0; id < 500; ++id) {
        std::array<int, 2> point{
            static_cast<int>(random() % 1000),
            static_cast<int>(random() % 1000)
        };
        assert(kd.insert(id, point));
        points[id] = point;
    }
    for (int id = 0; id < 500; id += 4) {
        assert(kd.erase(id));
        points.erase(id);
    }
    for (int query = 0; query < 200; ++query) {
        std::array<int, 2> lower{
            static_cast<int>(random() % 700),
            static_cast<int>(random() % 700)
        };
        std::array<int, 2> upper{
            lower[0] + static_cast<int>(random() % 300),
            lower[1] + static_cast<int>(random() % 300)
        };
        auto actual = kd.range_search(lower, upper);
        std::sort(actual.begin(), actual.end());
        std::vector<int> expected;
        for (const auto& [id, point] : points) {
            if (lower[0] <= point[0] && point[0] <= upper[0]
                && lower[1] <= point[1] && point[1] <= upper[1]) {
                expected.push_back(id);
            }
        }
        assert(actual == expected);
    }
    std::cout << "OK\n";
}
