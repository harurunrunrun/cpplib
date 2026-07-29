#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_GENIUS_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_GENIUS_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

namespace genius_detail {

template<class DistanceMatrix>
void validate_symmetric_distance(const DistanceMatrix& distance) {
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("vertex count does not fit in int");
    }
    for(std::size_t first = 0; first < n; ++first){
        for(std::size_t second = first + 1; second < n; ++second){
            if(distance[first][second] != distance[second][first]){
                throw std::invalid_argument(
                    "GENI/US needs a symmetric distance matrix"
                );
            }
        }
    }
}

inline void validate_permutation(
    const std::vector<int>& tour,
    std::size_t n
) {
    if(tour.size() != n){
        throw std::invalid_argument("tour must contain every vertex");
    }
    std::vector<unsigned char> used(n, 0);
    for(const int raw_vertex : tour){
        if(raw_vertex < 0 || static_cast<std::size_t>(raw_vertex) >= n){
            throw std::out_of_range("tour vertex is outside matrix");
        }
        const std::size_t vertex = static_cast<std::size_t>(raw_vertex);
        if(used[vertex]){
            throw std::invalid_argument("tour must be a permutation");
        }
        used[vertex] = 1;
    }
}

template<class DistanceMatrix>
long double cycle_cost(
    const DistanceMatrix& distance,
    const std::vector<int>& tour
) {
    long double result = 0;
    for(std::size_t index = 0; index < tour.size(); ++index){
        result += static_cast<long double>(
            distance[static_cast<std::size_t>(tour[index])]
                    [static_cast<std::size_t>(
                        tour[(index + 1) % tour.size()]
                    )]
        );
    }
    return result;
}

struct InsertionChoice {
    long double delta = std::numeric_limits<long double>::infinity();
    std::size_t first = 0;
    std::size_t second = 0;
    bool reverse_segment = false;
};

template<class DistanceMatrix>
InsertionChoice best_geni_insertion(
    const DistanceMatrix& distance,
    const std::vector<int>& tour,
    std::size_t vertex,
    std::size_t neighborhood_size
) {
    InsertionChoice choice;
    if(tour.empty()){
        choice.delta = 0;
        return choice;
    }
    std::vector<std::size_t> neighbor_position(tour.size());
    std::iota(neighbor_position.begin(), neighbor_position.end(), 0);
    std::stable_sort(
        neighbor_position.begin(), neighbor_position.end(),
        [&](std::size_t first, std::size_t second) {
            return distance[vertex][static_cast<std::size_t>(tour[first])]
                 < distance[vertex][static_cast<std::size_t>(tour[second])];
        }
    );
    neighbor_position.resize(
        std::min(neighborhood_size, neighbor_position.size())
    );
    std::sort(neighbor_position.begin(), neighbor_position.end());

    for(std::size_t first = 0; first < tour.size(); ++first){
        const std::size_t next = (first + 1) % tour.size();
        const std::size_t a = static_cast<std::size_t>(tour[first]);
        const std::size_t b = static_cast<std::size_t>(tour[next]);
        const long double delta =
            static_cast<long double>(distance[a][vertex])
            + static_cast<long double>(distance[vertex][b])
            - static_cast<long double>(distance[a][b]);
        if(delta < choice.delta){
            choice = {delta, first, first, false};
        }
    }
    if(tour.size() < 4) return choice;

    for(const std::size_t first : neighbor_position){
        for(const std::size_t second : neighbor_position){
            if(first >= second || second + 1 >= tour.size()) continue;
            const std::size_t a = static_cast<std::size_t>(tour[first]);
            const std::size_t b = static_cast<std::size_t>(tour[first + 1]);
            const std::size_t c = static_cast<std::size_t>(tour[second]);
            const std::size_t d = static_cast<std::size_t>(tour[second + 1]);
            const long double delta =
                static_cast<long double>(distance[a][vertex])
                + static_cast<long double>(distance[vertex][c])
                + static_cast<long double>(distance[b][d])
                - static_cast<long double>(distance[a][b])
                - static_cast<long double>(distance[c][d]);
            if(delta < choice.delta){
                choice = {delta, first, second, true};
            }
        }
    }
    return choice;
}

inline std::vector<int> apply_insertion(
    const std::vector<int>& tour,
    int vertex,
    const InsertionChoice& choice
) {
    if(tour.empty()) return {vertex};
    std::vector<int> result;
    result.reserve(tour.size() + 1);
    result.insert(
        result.end(), tour.begin(),
        tour.begin()
            + static_cast<std::vector<int>::difference_type>(choice.first + 1)
    );
    result.push_back(vertex);
    if(choice.reverse_segment){
        for(std::size_t position = choice.second + 1;
            position-- > choice.first + 1;){
            result.push_back(tour[position]);
        }
        result.insert(
            result.end(),
            tour.begin()
                + static_cast<std::vector<int>::difference_type>(
                    choice.second + 1
                ),
            tour.end()
        );
    }else{
        result.insert(
            result.end(),
            tour.begin()
                + static_cast<std::vector<int>::difference_type>(
                    choice.first + 1
                ),
            tour.end()
        );
    }
    return result;
}

}  // namespace genius_detail

template<class DistanceMatrix>
std::vector<int> geni_tour(
    const DistanceMatrix& distance,
    std::size_t neighborhood_size = 5,
    std::size_t start = 0
) {
    genius_detail::validate_symmetric_distance(distance);
    const std::size_t n = distance.size();
    if(neighborhood_size == 0){
        throw std::invalid_argument("neighborhood_size must be positive");
    }
    if(n == 0){
        if(start != 0) throw std::out_of_range("start is outside empty matrix");
        return {};
    }
    if(start >= n) throw std::out_of_range("start is outside matrix");
    std::vector<int> tour{static_cast<int>(start)};
    std::vector<unsigned char> inserted(n, 0);
    inserted[start] = 1;
    while(tour.size() < n){
        std::size_t selected = n;
        genius_detail::InsertionChoice selected_choice;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(inserted[vertex]) continue;
            const auto choice = genius_detail::best_geni_insertion(
                distance, tour, vertex, neighborhood_size
            );
            if(selected == n || choice.delta < selected_choice.delta
               || (choice.delta == selected_choice.delta
                   && vertex < selected)){
                selected = vertex;
                selected_choice = choice;
            }
        }
        tour = genius_detail::apply_insertion(
            tour, static_cast<int>(selected), selected_choice
        );
        inserted[selected] = 1;
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> unstringing_stringing_tour(
    const DistanceMatrix& distance,
    std::vector<int> tour,
    std::size_t maximum_passes = 8,
    std::size_t neighborhood_size = 5
) {
    genius_detail::validate_symmetric_distance(distance);
    genius_detail::validate_permutation(tour, distance.size());
    if(neighborhood_size == 0){
        throw std::invalid_argument("neighborhood_size must be positive");
    }
    if(tour.size() < 4) return tour;
    long double current_cost = genius_detail::cycle_cost(distance, tour);
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        bool improved = false;
        const std::vector<int> pass_order = tour;
        for(const int vertex : pass_order){
            const auto iterator = std::find(tour.begin(), tour.end(), vertex);
            const std::size_t position = static_cast<std::size_t>(
                iterator - tour.begin()
            );
            std::vector<int> reduced = tour;
            reduced.erase(
                reduced.begin()
                    + static_cast<std::vector<int>::difference_type>(position)
            );
            const auto choice = genius_detail::best_geni_insertion(
                distance, reduced, static_cast<std::size_t>(vertex),
                neighborhood_size
            );
            std::vector<int> candidate =
                genius_detail::apply_insertion(reduced, vertex, choice);
            const long double candidate_cost =
                genius_detail::cycle_cost(distance, candidate);
            if(candidate_cost < current_cost){
                tour = std::move(candidate);
                current_cost = candidate_cost;
                improved = true;
            }
        }
        if(!improved) break;
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> genius_tour(
    const DistanceMatrix& distance,
    std::size_t neighborhood_size = 5,
    std::size_t maximum_us_passes = 8,
    std::size_t start = 0
) {
    std::vector<int> tour =
        geni_tour(distance, neighborhood_size, start);
    return unstringing_stringing_tour(
        distance, std::move(tour), maximum_us_passes, neighborhood_size
    );
}

template<class DistanceMatrix>
std::vector<int> record_to_record_travel_tour(
    const DistanceMatrix& distance,
    std::vector<int> tour,
    long double maximum_deviation,
    std::size_t maximum_passes = 20
) {
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("vertex count does not fit in int");
    }
    genius_detail::validate_permutation(tour, distance.size());
    if(!(maximum_deviation >= 0) || !std::isfinite(maximum_deviation)){
        throw std::invalid_argument(
            "maximum_deviation must be finite and nonnegative"
        );
    }
    if(tour.size() < 4) return tour;
    std::vector<int> record_tour = tour;
    long double record = genius_detail::cycle_cost(distance, tour);
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        bool accepted = false;
        for(std::size_t first = 0; first + 2 < tour.size(); ++first){
            for(std::size_t second = first + 2; second < tour.size(); ++second){
                if(first == 0 && second + 1 == tour.size()) continue;
                std::vector<int> candidate = tour;
                std::reverse(
                    candidate.begin()
                        + static_cast<std::vector<int>::difference_type>(
                            first + 1
                        ),
                    candidate.begin()
                        + static_cast<std::vector<int>::difference_type>(
                            second + 1
                        )
                );
                const long double candidate_cost =
                    genius_detail::cycle_cost(distance, candidate);
                if(candidate_cost <= record + maximum_deviation){
                    tour = std::move(candidate);
                    accepted = true;
                    if(candidate_cost < record){
                        record = candidate_cost;
                        record_tour = tour;
                    }
                }
            }
        }
        if(!accepted) break;
    }
    return record_tour;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_GENIUS_SEARCH_HPP_INCLUDED
