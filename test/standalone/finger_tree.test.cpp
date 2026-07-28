// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <random>
#include <string>
#include <utility>

#include "../../src/structure/bbst/sequence/detail/standard_finger_tree.hpp"
#include "../../src/structure/bbst/sequence/finger_tree.hpp"

struct Sum {
    using S = long long;
    constexpr S op(S a, S b) const { return a + b; }
    constexpr S e() const { return 0; }
};
constexpr Sum sum{};
struct StringMonoid {
    using S = std::string;
    S op(S a, const S& b) const { a += b; return a; }
    S e() const { return {}; }
};
constexpr StringMonoid string_monoid{};


int main() {
    FingerTree<sum> tree;
    std::deque<long long> expected;
    long long expected_sum = 0;
    std::mt19937 random(20260729);

    for (int step = 0; step < 20000; ++step) {
        const int operation = static_cast<int>(random() % 7);
        if (operation == 0) {
            const long long value = 1 + random() % 100;
            tree.push_front(value);
            expected.push_front(value);
            expected_sum += value;
        } else if (operation == 1) {
            const long long value = 1 + random() % 100;
            tree.push_back(value);
            expected.push_back(value);
            expected_sum += value;
        } else if (operation == 2 && !expected.empty()) {
            expected_sum -= expected.front();
            expected.pop_front();
            tree.pop_front();
        } else if (operation == 3 && !expected.empty()) {
            expected_sum -= expected.back();
            expected.pop_back();
            tree.pop_back();
        } else if (operation == 4) {
            const std::size_t index = expected.empty()
                ? 0 : static_cast<std::size_t>(random() % (expected.size() + 1));
            auto [left, right] = std::move(tree).split(index);
            assert(left.size() == index);
            assert(right.size() == expected.size() - index);
            for (std::size_t i = 0; i < index; ++i) assert(left.at(i) == expected[i]);
            for (std::size_t i = index; i < expected.size(); ++i) {
                assert(right.at(i - index) == expected[i]);
            }
            left.concatenate(std::move(right));
            tree = std::move(left);
        } else if (operation == 5) {
            const long long threshold = expected_sum == 0
                ? 0 : static_cast<long long>(random() % (expected_sum + 2));
            auto [left, right] = std::move(tree).split_by(
                [threshold](long long prefix) { return prefix >= threshold; }
            );
            std::size_t index = 0;
            long long prefix = 0;
            while (index < expected.size()) {
                const long long next = prefix + expected[index];
                if (next >= threshold) break;
                prefix = next;
                ++index;
            }
            assert(left.size() == index);
            assert(right.size() == expected.size() - index);
            left.concatenate(std::move(right));
            tree = std::move(left);
        } else if (!expected.empty()) {
            const std::size_t index = static_cast<std::size_t>(
                random() % expected.size()
            );
            assert(tree.at(index) == expected[index]);
        }

        assert(tree.size() == expected.size());
        assert(tree.empty() == expected.empty());
        assert(tree.fold() == expected_sum);
        if (!expected.empty()) {
            assert(tree.front() == expected.front());
            assert(tree.back() == expected.back());
        }
    }

    FingerTree<sum> suffix;
    for (long long value = 1; value <= 1000; ++value) suffix.push_back(value);
    FingerTree<string_monoid> text;
    text.push_back("b");
    text.push_front("a");
    text.push_back("c");
    assert(text.fold() == "abc");
    auto [text_left, text_right] = std::move(text).split_by(
        [](const std::string& prefix) { return prefix.size() >= 2; }
    );
    assert(text_left.fold() == "a");
    assert(text_right.fold() == "bc");
    text_left.concatenate(std::move(text_right));
    assert(text_left.fold() == "abc");

    const std::size_t old_size = tree.size();
    tree.concatenate(std::move(suffix));
    assert(tree.size() == old_size + 1000);
    for (std::size_t i = 0; i < 1000; ++i) {
        assert(tree.at(old_size + i) == static_cast<long long>(i + 1));
    }
    std::cout << "OK\n";
}
