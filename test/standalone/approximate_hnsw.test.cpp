// competitive-verifier: STANDALONE
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "../../src/approximate/nearest_neighbor/hnsw.hpp"
namespace {
using Point = std::array<long double, 2>;
using Index = approximate::nearest_neighbor::HnswIndex<long double, 2>;

bool same_neighbors(const std::vector<approximate::nearest_neighbor::AnnNeighbor>& first,
                    const std::vector<approximate::nearest_neighbor::AnnNeighbor>& second) {
    if(first.size() != second.size()) return false;
    for(std::size_t index = 0; index < first.size(); ++index){
        if(first[index].index != second[index].index
           || first[index].squared_distance != second[index].squared_distance) return false;
    }
    return true;
}

void audit_edge_cases() {
    Index empty(2, 4, 1);
    assert(empty.empty());
    bool rejected = false;
    try { static_cast<void>(empty.nearest(Point{0, 0}, 1, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    const std::vector<Point> points{{0, 0}, {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {8, 8}};
    Index first(2, 6, 42);
    Index second(2, 6, 42);
    for(std::size_t index = 0; index < points.size(); ++index){
        assert(first.insert(points[index]) == index);
        assert(second.insert(points[index]) == index);
        assert(first.point(index) == points[index]);
    }
    const auto exact = first.nearest(Point{0, 0}, 4, 4, true);
    assert(exact[0].index == 0 && exact[1].index == 1 && exact[2].index == 2);
    for(std::size_t k = 1; k <= points.size(); ++k){
        const auto found = first.nearest(Point{2, 1}, k, k);
        assert(found.size() == k);
        assert(std::is_sorted(found.begin(), found.end(),
                              approximate::nearest_neighbor::detail::neighbor_less));
        assert(same_neighbors(found, second.nearest(Point{2, 1}, k, k)));
    }
    const auto huge_ef = first.nearest(
        Point{2, 1}, 1, std::numeric_limits<std::size_t>::max());
    assert(huge_ef.size() == 1);
    for(std::size_t index = 0; index < points.size(); ++index){
        const auto neighbors = first.neighbors_of(index, 3, 4);
        assert(neighbors.size() == 3);
        assert(std::none_of(neighbors.begin(), neighbors.end(),
            [index](const auto& neighbor) { return neighbor.index == index; }));
    }

    rejected = false;
    try { static_cast<void>(first.point(points.size())); }
    catch(const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.nearest(Point{0, 0}, 0, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.nearest(Point{0, 0}, points.size() + 1,
                                          points.size() + 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.nearest(Point{0, 0}, 2, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.neighbors_of(points.size(), 1, 2)); }
    catch(const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.neighbors_of(0, 1, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    rejected = false;
    try {
        static_cast<void>(first.nearest(
            Point{std::numeric_limits<long double>::infinity(), 0}, 1, 1));
    }catch(const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    const std::size_t previous_size = first.size();
    rejected = false;
    try { first.insert(Point{std::numeric_limits<long double>::infinity(), 0}); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected && first.size() == previous_size);

    const long double large = std::numeric_limits<long double>::max() / 4.0L;
    Index overflow_index(2, 4, 9);
    assert(overflow_index.insert(Point{-large, 0}) == 0);
    bool overflowed = false;
    try { overflow_index.insert(Point{large, 0}); }
    catch(const std::overflow_error&) { overflowed = true; }
    assert(overflowed && overflow_index.size() == 1);
    overflowed = false;
    try { static_cast<void>(overflow_index.nearest(Point{large, 0}, 1, 1, true)); }
    catch(const std::overflow_error&) { overflowed = true; }
    assert(overflowed && overflow_index.size() == 1);

    const long double extent =
        std::sqrt(std::numeric_limits<long double>::max()) * 0.75L;
    Index rollback_index(2, 4, 23);
    assert(rollback_index.insert(Point{0, 0}) == 0);
    assert(rollback_index.insert(Point{-extent, 0}) == 1);
    const auto before_failed_insert =
        rollback_index.nearest(Point{0, 0}, 2, 2, true);
    overflowed = false;
    try { rollback_index.insert(Point{extent, 0}); }
    catch(const std::overflow_error&) { overflowed = true; }
    assert(overflowed && rollback_index.size() == 2);
    assert(rollback_index.point(0) == Point({0, 0}));
    assert(rollback_index.point(1) == Point({-extent, 0}));
    assert(same_neighbors(before_failed_insert,
                          rollback_index.nearest(Point{0, 0}, 2, 2, true)));
    assert(rollback_index.nearest(Point{0, 0}, 1, 2).size() == 1);

    first.clear();
    assert(first.empty());
    assert(first.insert(Point{7, 9}) == 0);
    assert(first.nearest(Point{7, 9}, 1, 1)[0].index == 0);

    rejected = false;
    try { Index invalid(4, 3, 0); static_cast<void>(invalid); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
}
}  // namespace

int main() {
    audit_edge_cases();
    using P = std::array<long double, 2>;
    std::size_t n;
    std::size_t q;
    std::size_t k;
    if(!(std::cin >> n >> q >> k)) return 0;
    approximate::nearest_neighbor::HnswIndex<long double, 2> index(8, 32, 20260713);
    std::vector<P> points(n);
    for(auto& point: points){
        std::cin >> point[0] >> point[1];
        index.insert(point);
    }
    assert(index.size() == n);
    while(q-- > 0){
        P query{};
        std::cin >> query[0] >> query[1];
        const auto found = index.nearest(query, k, n);
        const auto exact = index.nearest(query, k, k, true);
        assert(found.size() == k);
        assert(std::is_sorted(found.begin(), found.end(),
                              approximate::nearest_neighbor::detail::neighbor_less));
        std::size_t hit = 0;
        for(const auto& expected: exact){
            hit += static_cast<std::size_t>(std::any_of(
                found.begin(), found.end(),
                [&](const auto& neighbor) { return neighbor.index == expected.index; }));
        }
        assert(hit * 2 >= k);
        for(std::size_t result_index = 0; result_index < k; ++result_index){
            std::cout << exact[result_index].index
                      << (result_index + 1 == k ? '\n' : ' ');
        }
    }
    const auto self = index.neighbors_of(0, k, n, true);
    assert(self.size() == k);
    for(const auto& neighbor: self) assert(neighbor.index != 0);
    bool rejected = false;
    try {
        approximate::nearest_neighbor::HnswIndex<double, 2> invalid(1, 2);
        static_cast<void>(invalid);
    }catch(const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
