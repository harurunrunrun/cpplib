// competitive-verifier: STANDALONE
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/approximate/nearest_neighbor/nn_descent.hpp"
#include "../../src/approximate/nearest_neighbor/ann_common.hpp"
namespace {
using Point = std::array<long double, 2>;
using Descent = approximate::nearest_neighbor::NnDescent<long double, 2>;

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
    Descent default_descent;
    assert(default_descent.size() == 0 && default_descent.neighbor_count() == 0);

    std::mt19937_64 empty_random(1);
    Descent empty({}, 0, 1, 1, empty_random);
    assert(empty.size() == 0);
    bool rejected = false;
    try { static_cast<void>(empty.nearest(Point{0, 0}, 1, 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    std::mt19937_64 single_random(2);
    Descent single({Point{3, 4}}, 0, 1, 1, single_random);
    const auto only = single.nearest(Point{3, 4}, 1, 1);
    assert(only.size() == 1 && only[0].index == 0);
    assert(single.neighbors(0).empty());

    const std::vector<Point> points{{0, 0}, {0, 0}, {1, 0}, {-1, 0}, {0, 1}};
    std::mt19937_64 first_random(42);
    std::mt19937_64 second_random(42);
    Descent first(points, 3, 8, 4, first_random);
    Descent second(points, 3, 8, 4, second_random);
    for(std::size_t index = 0; index < points.size(); ++index){
        assert(first.neighbors(index).size() == 3);
        assert(std::is_sorted(first.neighbors(index).begin(), first.neighbors(index).end(),
                              approximate::nearest_neighbor::detail::neighbor_less));
        assert(std::none_of(first.neighbors(index).begin(), first.neighbors(index).end(),
            [index](const auto& neighbor) { return neighbor.index == index; }));
        for(std::size_t left = 0; left < first.neighbors(index).size(); ++left){
            for(std::size_t right = left + 1; right < first.neighbors(index).size(); ++right){
                assert(first.neighbors(index)[left].index
                       != first.neighbors(index)[right].index);
            }
        }
        assert(same_neighbors(first.neighbors(index), second.neighbors(index)));
    }
    const auto exact = first.nearest(Point{0, 0}, 4, 4, true);
    assert(exact[0].index == 0 && exact[1].index == 1 && exact[2].index == 2);
    assert(same_neighbors(first.nearest(Point{2, 1}, 3, points.size()),
                          first.nearest(Point{2, 1}, 3, 3, true)));
    assert(same_neighbors(first.nearest(Point{2, 1}, 3, 3),
                          second.nearest(Point{2, 1}, 3, 3)));

    rejected = false;
    try { static_cast<void>(first.point(points.size())); }
    catch(const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { static_cast<void>(first.neighbors(points.size())); }
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
    try {
        static_cast<void>(first.nearest(
            Point{std::numeric_limits<long double>::infinity(), 0}, 1, 1));
    }catch(const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try { Descent invalid(points, 0, 1, 1, first_random); static_cast<void>(invalid); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { Descent invalid(points, 2, 0, 2, first_random); static_cast<void>(invalid); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { Descent invalid(points, 2, 1, 1, first_random); static_cast<void>(invalid); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    const long double large = std::numeric_limits<long double>::max() / 4.0L;
    std::mt19937_64 overflow_random(9);
    Descent overflow_descent({Point{-large, 0}}, 0, 1, 1, overflow_random);
    bool overflowed = false;
    try { static_cast<void>(overflow_descent.nearest(Point{large, 0}, 1, 1)); }
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
    const std::size_t graph_degree = std::min<std::size_t>(8, n - 1);
    approximate::nearest_neighbor::NnDescent<long double, 2> descent(
        points, graph_degree, 12, std::min<std::size_t>(32, n - 1), random);
    for(std::size_t index = 0; index < n; ++index){
        const auto& neighbors = descent.neighbors(index);
        assert(std::is_sorted(neighbors.begin(), neighbors.end(),
                              approximate::nearest_neighbor::detail::neighbor_less));
        for(const auto& neighbor: neighbors) assert(neighbor.index != index);
    }
    while(q-- > 0){
        P query{};
        std::cin >> query[0] >> query[1];
        const auto found = descent.nearest(query, k, n);
        const auto exact = descent.nearest(query, k, k, true);
        for(std::size_t index = 0; index < k; ++index){
            assert(found[index].index == exact[index].index);
            std::cout << exact[index].index << (index + 1 == k ? '\n' : ' ');
        }
    }
    bool rejected = false;
    try { static_cast<void>(descent.nearest(P{0, 0}, k, k - 1)); }
    catch(const std::invalid_argument&) { rejected = true; }
    assert(rejected);
}
