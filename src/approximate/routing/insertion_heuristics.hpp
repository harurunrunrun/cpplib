#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_INSERTION_HEURISTICS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_INSERTION_HEURISTICS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"
#include "tour_cost.hpp"

namespace approximate::routing {

namespace internal {

template<class DistanceMatrix>
auto insertion_delta(
    const DistanceMatrix& distance,
    const std::vector<int>& tour,
    int vertex,
    std::size_t position
) -> distance_cost_t<DistanceMatrix> {
    const std::size_t current = static_cast<std::size_t>(tour[position]);
    const std::size_t previous = static_cast<std::size_t>(
        tour[(position + tour.size() - 1) % tour.size()]
    );
    const std::size_t inserted = static_cast<std::size_t>(vertex);
    return distance[previous][inserted] + distance[inserted][current]
        - distance[previous][current];
}

template<class DistanceMatrix>
std::pair<std::size_t, distance_cost_t<DistanceMatrix>>
cheapest_position(
    const DistanceMatrix& distance,
    const std::vector<int>& tour,
    int vertex
){
    std::size_t best_position = 0;
    auto best_delta = insertion_delta(distance, tour, vertex, 0);
    for(std::size_t position = 1; position < tour.size(); ++position){
        const auto delta = insertion_delta(distance, tour, vertex, position);
        if(delta < best_delta){
            best_delta = delta;
            best_position = position;
        }
    }
    return {best_position, best_delta};
}

template<class DistanceMatrix>
std::vector<int> singleton_seed(
    const DistanceMatrix& distance,
    std::size_t start
){
    const std::size_t n = validate_distance_matrix(distance);
    if(n == 0){
        if(start != 0) throw std::out_of_range("start is outside an empty matrix");
        return {};
    }
    if(start >= n) throw std::out_of_range("start vertex is outside the matrix");
    return {static_cast<int>(start)};
}

}  // namespace internal

template<class DistanceMatrix>
std::vector<int> nearest_neighbor_tour(
    const DistanceMatrix& distance,
    std::size_t start = 0
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n == 0){
        if(start != 0) throw std::out_of_range("start is outside an empty matrix");
        return {};
    }
    if(start >= n) throw std::out_of_range("start vertex is outside the matrix");

    std::vector<int> tour;
    tour.reserve(n);
    std::vector<unsigned char> used(n, 0);
    std::size_t current = start;
    used[current] = 1;
    tour.push_back(static_cast<int>(current));
    while(tour.size() < n){
        std::size_t next = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            if(next == n || distance[current][vertex] < distance[current][next]){
                next = vertex;
            }
        }
        used[next] = 1;
        tour.push_back(static_cast<int>(next));
        current = next;
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> repeated_nearest_neighbor_tour(
    const DistanceMatrix& distance
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n == 0) return {};
    std::vector<int> best = nearest_neighbor_tour(distance, 0);
    auto best_cost = tour_cost(distance, best);
    for(std::size_t start = 1; start < n; ++start){
        std::vector<int> candidate = nearest_neighbor_tour(distance, start);
        const auto candidate_cost = tour_cost(distance, candidate);
        if(candidate_cost < best_cost){
            best = std::move(candidate);
            best_cost = candidate_cost;
        }
    }
    return best;
}

template<class DistanceMatrix>
std::vector<int> nearest_insertion_tour(
    const DistanceMatrix& distance,
    std::size_t start = 0
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    std::vector<int> tour = internal::singleton_seed(distance, start);
    if(n < 2) return tour;
    std::vector<unsigned char> used(n, 0);
    std::vector<internal::distance_cost_t<DistanceMatrix>> nearest(n);
    used[start] = 1;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        nearest[vertex] = distance[start][vertex];
    }

    while(tour.size() < n){
        std::size_t selected = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            if(selected == n || nearest[vertex] < nearest[selected]) selected = vertex;
        }
        const auto [position, ignored_delta] = internal::cheapest_position(
            distance,
            tour,
            static_cast<int>(selected)
        );
        static_cast<void>(ignored_delta);
        using Difference = std::vector<int>::difference_type;
        tour.insert(
            tour.begin() + static_cast<Difference>(position),
            static_cast<int>(selected)
        );
        used[selected] = 1;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(
                used[vertex] == 0
                && distance[selected][vertex] < nearest[vertex]
            ) nearest[vertex] = distance[selected][vertex];
        }
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> cheapest_insertion_tour(
    const DistanceMatrix& distance,
    std::size_t start = 0
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    std::vector<int> tour = internal::singleton_seed(distance, start);
    if(n < 2) return tour;
    std::vector<unsigned char> used(n, 0);
    used[start] = 1;

    while(tour.size() < n){
        std::size_t selected = n;
        std::size_t selected_position = 0;
        internal::distance_cost_t<DistanceMatrix> selected_delta{};
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            const auto [position, delta] = internal::cheapest_position(
                distance,
                tour,
                static_cast<int>(vertex)
            );
            if(selected == n || delta < selected_delta){
                selected = vertex;
                selected_position = position;
                selected_delta = delta;
            }
        }
        using Difference = std::vector<int>::difference_type;
        tour.insert(
            tour.begin() + static_cast<Difference>(selected_position),
            static_cast<int>(selected)
        );
        used[selected] = 1;
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> farthest_insertion_tour(
    const DistanceMatrix& distance,
    std::size_t start = 0
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    std::vector<int> tour = internal::singleton_seed(distance, start);
    if(n < 2) return tour;
    std::vector<unsigned char> used(n, 0);
    std::vector<internal::distance_cost_t<DistanceMatrix>> nearest(n);
    used[start] = 1;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        nearest[vertex] = distance[start][vertex];
    }

    while(tour.size() < n){
        std::size_t selected = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            if(selected == n || nearest[selected] < nearest[vertex]) selected = vertex;
        }
        const auto [position, ignored_delta] = internal::cheapest_position(
            distance,
            tour,
            static_cast<int>(selected)
        );
        static_cast<void>(ignored_delta);
        using Difference = std::vector<int>::difference_type;
        tour.insert(
            tour.begin() + static_cast<Difference>(position),
            static_cast<int>(selected)
        );
        used[selected] = 1;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(
                used[vertex] == 0
                && distance[selected][vertex] < nearest[vertex]
            ) nearest[vertex] = distance[selected][vertex];
        }
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> regret_insertion_tour(
    const DistanceMatrix& distance,
    std::size_t start = 0,
    std::size_t regret_k = 2
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(regret_k == 0) throw std::invalid_argument("regret_k must be positive");
    std::vector<int> tour = internal::singleton_seed(distance, start);
    if(n < 2) return tour;
    std::vector<unsigned char> used(n, 0);
    used[start] = 1;

    while(tour.size() < n){
        std::size_t selected = n;
        std::size_t selected_position = 0;
        internal::distance_cost_t<DistanceMatrix> selected_regret{};
        internal::distance_cost_t<DistanceMatrix> selected_best{};
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            std::vector<std::pair<internal::distance_cost_t<DistanceMatrix>, std::size_t>>
                choices;
            choices.reserve(tour.size());
            for(std::size_t position = 0; position < tour.size(); ++position){
                choices.emplace_back(
                    internal::insertion_delta(
                        distance,
                        tour,
                        static_cast<int>(vertex),
                        position
                    ),
                    position
                );
            }
            std::sort(choices.begin(), choices.end());
            const std::size_t kth = std::min(regret_k, choices.size()) - 1;
            const auto regret = choices[kth].first - choices[0].first;
            if(
                selected == n || selected_regret < regret
                || (!(regret < selected_regret) && choices[0].first < selected_best)
            ){
                selected = vertex;
                selected_position = choices[0].second;
                selected_regret = regret;
                selected_best = choices[0].first;
            }
        }
        using Difference = std::vector<int>::difference_type;
        tour.insert(
            tour.begin() + static_cast<Difference>(selected_position),
            static_cast<int>(selected)
        );
        used[selected] = 1;
    }
    return tour;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_INSERTION_HEURISTICS_HPP_INCLUDED
