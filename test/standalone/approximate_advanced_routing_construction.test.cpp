// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/routing/genius_search.hpp"
#include "../../src/approximate/routing/one_tree_heuristics.hpp"
#include "../../src/approximate/routing/vehicle_routing_construction.hpp"

namespace {

bool is_permutation(const std::vector<int>& tour, std::size_t n) {
    if(tour.size() != n) return false;
    std::vector<int> sorted = tour;
    std::sort(sorted.begin(), sorted.end());
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(sorted[vertex] != static_cast<int>(vertex)) return false;
    }
    return true;
}

long double cycle_cost(
    const std::vector<std::vector<long long>>& distance,
    const std::vector<int>& tour
) {
    long double result = 0;
    for(std::size_t index = 0; index < tour.size(); ++index){
        result += distance[static_cast<std::size_t>(tour[index])]
                          [static_cast<std::size_t>(
                              tour[(index + 1) % tour.size()]
                          )];
    }
    return result;
}

long double exact_tsp(
    const std::vector<std::vector<long long>>& distance
) {
    std::vector<int> order(distance.size() - 1);
    std::iota(order.begin(), order.end(), 1);
    long double answer = std::numeric_limits<long double>::infinity();
    do {
        std::vector<int> tour{0};
        tour.insert(tour.end(), order.begin(), order.end());
        answer = std::min(answer, cycle_cost(distance, tour));
    } while(std::next_permutation(order.begin(), order.end()));
    return answer;
}

void validate_routes(
    const std::vector<std::vector<int>>& routes,
    const std::vector<long double>& demand,
    long double capacity
) {
    std::vector<int> seen(demand.size(), 0);
    seen[0] = 1;
    for(const auto& route : routes){
        long double load = 0;
        for(const int vertex : route){
            assert(vertex > 0);
            assert(static_cast<std::size_t>(vertex) < demand.size());
            assert(++seen[static_cast<std::size_t>(vertex)] == 1);
            load += demand[static_cast<std::size_t>(vertex)];
        }
        assert(load <= capacity);
    }
    for(const int count : seen) assert(count == 1);
}

template<class Exception, class Function>
bool throws(Function&& function) {
    try {
        function();
    }catch(const Exception&) {
        return true;
    }
    return false;
}

}  // namespace

int main() {
    std::uint64_t seed = 0;
    int rounds = 0;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const std::size_t n = 5 + random() % 4;
        std::vector<std::pair<long long, long long>> point(n);
        for(auto& [x, y] : point){
            x = static_cast<long long>(random() % 101);
            y = static_cast<long long>(random() % 101);
        }
        std::vector<std::vector<long long>> distance(
            n, std::vector<long long>(n)
        );
        for(std::size_t first = 0; first < n; ++first){
            for(std::size_t second = 0; second < n; ++second){
                distance[first][second] =
                    std::llabs(point[first].first - point[second].first)
                    + std::llabs(point[first].second - point[second].second);
            }
        }

        const long double optimum = exact_tsp(distance);
        const auto tree =
            approximate::routing::minimum_one_tree(distance);
        assert(tree.edges.size() == n);
        assert(
            std::accumulate(tree.degree.begin(), tree.degree.end(), 0)
            == static_cast<int>(2 * n)
        );
        assert(tree.lower_bound <= optimum + 1e-9L);
        const auto held_karp =
            approximate::routing::held_karp_subgradient_heuristic(
                distance, optimum, 80
            );
        assert(held_karp.iterations >= 1);
        assert(held_karp.lower_bound <= optimum + 1e-7L);
        assert(held_karp.vertex_penalty.size() == n);

        const auto geni = approximate::routing::geni_tour(distance, 4);
        const auto us =
            approximate::routing::unstringing_stringing_tour(
                distance, geni, 4, 4
            );
        const auto genius =
            approximate::routing::genius_tour(distance, 4, 4);
        assert(is_permutation(geni, n));
        assert(is_permutation(us, n));
        assert(is_permutation(genius, n));
        assert(cycle_cost(distance, us) <= cycle_cost(distance, geni));

        std::vector<int> shuffled(n);
        std::iota(shuffled.begin(), shuffled.end(), 0);
        std::shuffle(shuffled.begin(), shuffled.end(), random);
        const auto record =
            approximate::routing::record_to_record_travel_tour(
                distance, shuffled, 10, 3
            );
        assert(is_permutation(record, n));
        assert(cycle_cost(distance, record) <= cycle_cost(distance, shuffled));

        std::vector<long double> demand(n, 1);
        demand[0] = 0;
        const std::size_t vehicles = (n - 1 + 2) / 3;
        const auto fisher =
            approximate::routing::fisher_jaikumar_routes(
                distance, demand, 3, vehicles
            );
        validate_routes(fisher, demand, 3);

        std::vector<approximate::routing::RoutingTimeWindow> window(n);
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            window[vertex] = {
                0.0L, 100000.0L, vertex == 0 ? 0.0L : 1.0L
            };
        }
        validate_routes(
            approximate::routing::mole_jameson_routes(
                distance, demand, 3, window
            ),
            demand, 3
        );
        validate_routes(
            approximate::routing::solomon_i1_routes(
                distance, demand, 3, window
            ),
            demand, 3
        );
        validate_routes(
            approximate::routing::solomon_i2_routes(
                distance, demand, 3, window
            ),
            demand, 3
        );
        validate_routes(
            approximate::routing::solomon_i3_routes(
                distance, demand, 3, window
            ),
            demand, 3
        );
    }

    assert(throws<std::invalid_argument>([] {
        const std::vector<std::vector<int>> too_small{{0, 1}, {1, 0}};
        static_cast<void>(
            approximate::routing::minimum_one_tree(too_small)
        );
    }));
    assert(throws<std::invalid_argument>([] {
        const std::vector<std::vector<int>> asymmetric{
            {0, 1, 2}, {2, 0, 3}, {2, 3, 0}
        };
        static_cast<void>(approximate::routing::geni_tour(asymmetric));
    }));
    assert(throws<std::domain_error>([] {
        const std::vector<std::vector<int>> distance{
            {0, 1}, {1, 0}
        };
        static_cast<void>(
            approximate::routing::fisher_jaikumar_routes(
                distance, std::vector<long double>{0, 2}, 1, 1
            )
        );
    }));
    std::cout << "OK\n";
}
