// competitive-verifier: STANDALONE
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/approximate/nearest_neighbor/random_projection_forest.hpp"
namespace {
using Point = std::array<long double, 2>;
using Forest = approximate::nearest_neighbor::RandomProjectionForest<long double, 2>;

bool same_neighbors(const std::vector<approximate::nearest_neighbor::AnnNeighbor>& first,
                    const std::vector<approximate::nearest_neighbor::AnnNeighbor>& second) {
    if(first.size() != second.size()) return false;
    for(std::size_t index = 0; index < first.size(); ++index){
        if(first[index].index != second[index].index
           || first[index].squared_distance != second[index].squared_distance) return false;
    }
    return true;
}

struct ConstantEngine {
    using result_type = std::uint64_t;
    static constexpr result_type min() noexcept { return 0; }
    static constexpr result_type max() noexcept { return std::numeric_limits<result_type>::max(); }
    result_type operator()() const noexcept { return min(); }
};

void audit_edge_cases() {
    Forest default_forest;
    assert(default_forest.empty() && default_forest.tree_count() == 0);

    std::mt19937_64 empty_random(1);
    Forest empty({}, 2, 1, empty_random);
    assert(empty.empty());
    bool rejected = false;
    try { static_cast<void>(empty.nearest(Point{0, 0}, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    std::mt19937_64 single_random(2);
    Forest single({Point{3, 4}}, 1, 8, single_random);
    const auto only = single.nearest(Point{3, 4}, 1);
    assert(only.size() == 1 && only[0].index == 0);
    rejected = false;
    try { static_cast<void>(single.point(1)); }
    catch(const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(single.neighbors_of(0, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    const std::vector<Point> points{{0, 0}, {0, 0}, {1, 0}, {-1, 0}, {0, 1}};
    std::mt19937_64 first_random(42);
    std::mt19937_64 second_random(42);
    Forest first(points, 4, 1, first_random);
    Forest second(points, 4, 1, second_random);
    const auto approximate = first.nearest(Point{0, 0}, 4);
    const auto exact = first.nearest(Point{0, 0}, 4, true);
    assert(approximate.size() == 4);
    assert(exact[0].index == 0 && exact[1].index == 1 && exact[2].index == 2);
    assert(same_neighbors(approximate, second.nearest(Point{0, 0}, 4)));
    const auto excluded = first.neighbors_of(0, 4);
    assert(excluded.size() == 4);
    assert(std::none_of(excluded.begin(), excluded.end(),
        [](const auto& neighbor) { return neighbor.index == 0; }));

    const auto before_failed_reset = first.nearest(Point{2, 3}, 4, true);
    const std::size_t old_size = first.size();
    const std::size_t old_tree_count = first.tree_count();
    const std::size_t old_leaf_size = first.leaf_size();
    std::mt19937_64 failure_random(99);
    bool length_error = false;
    try {
        first.reset(points, std::numeric_limits<std::size_t>::max(), 1, failure_random);
    }catch(const std::length_error&) {
        length_error = true;
    }
    assert(length_error && first.size() == old_size);
    assert(first.tree_count() == old_tree_count && first.leaf_size() == old_leaf_size);
    assert(same_neighbors(before_failed_reset, first.nearest(Point{2, 3}, 4, true)));
    rejected = false;
    try { static_cast<void>(first.neighbors_of(points.size(), 1)); }
    catch(const std::out_of_range&) { rejected = true; }
    assert(rejected);

    std::mt19937_64 fallback_random(123);
    Forest fallback(points, 1, 1, fallback_random);
    const Point far_query{100, 37};
    const auto completed = fallback.nearest(far_query, 4);
    assert(completed.size() == 4);
    assert(same_neighbors(completed, fallback.nearest(far_query, 4, true)));

    ConstantEngine constant_random;
    Forest constant(points, 2, 1, constant_random);
    assert(constant.size() == points.size());

    const long double large = std::numeric_limits<long double>::max() / 4.0L;
    std::mt19937_64 overflow_random(7);
    Forest overflow_forest({Point{-large, 0}}, 1, 1, overflow_random);
    bool overflowed = false;
    try { static_cast<void>(overflow_forest.nearest(Point{large, 0}, 1, true)); }
    catch(const std::overflow_error&) { overflowed = true; }
    assert(overflowed);
}
}  // namespace

int main() {
    audit_edge_cases();
    using P = std::array<long double, 2>;
    std::size_t n;
    std::size_t q;
    std::size_t k;
    if(!(std::cin >> n >> q >> k)) return 0;
    std::vector<P> points(n);
    for(auto& point: points) std::cin >> point[0] >> point[1];
    std::mt19937_64 random(20260713);
    approximate::nearest_neighbor::RandomProjectionForest<long double, 2> forest(
        points, 12, 8, random);
    while(q-- > 0){
        P query{};
        std::cin >> query[0] >> query[1];
        const auto found = forest.nearest(query, k);
        const auto exact = forest.nearest(query, k, true);
        assert(found.size() == k);
        assert(std::is_sorted(found.begin(), found.end(),
                              approximate::nearest_neighbor::detail::neighbor_less));
        std::size_t hit = 0;
        for(const auto& expected: exact){
            hit += static_cast<std::size_t>(std::any_of(
                found.begin(), found.end(),
                [&](const auto& neighbor) { return neighbor.index == expected.index; }));
        }
        assert(hit * 2 >= found.size());
        for(std::size_t index = 0; index < k; ++index){
            std::cout << exact[index].index << (index + 1 == k ? '\n' : ' ');
        }
    }
    const auto self = forest.neighbors_of(0, std::min(k, n - 1), true);
    for(const auto& neighbor: self) assert(neighbor.index != 0);
    bool rejected = false;
    try { static_cast<void>(forest.nearest(P{0, 0}, 0)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try {
        static_cast<void>(forest.nearest(
            P{std::numeric_limits<long double>::infinity(), 0}, 1));
    }catch(const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
