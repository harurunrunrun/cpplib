#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_ADVANCED_LOCAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_ADVANCED_LOCAL_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"
#include "local_moves.hpp"
#include "tour_cost.hpp"

namespace approximate::routing {

inline void adjacent_swap(std::vector<int>& tour, std::size_t first) {
    if(first >= tour.size() || tour.size() - first < 2){
        throw std::out_of_range("adjacent_swap needs first + 1 < tour.size()");
    }
    std::swap(tour[first], tour[first + 1]);
}

inline void pair_swap(
    std::vector<int>& tour,
    std::size_t first,
    std::size_t second
) {
    if(first >= tour.size() || tour.size() - first < 2
       || second >= tour.size() || tour.size() - second < 2){
        throw std::out_of_range("pair_swap needs two complete adjacent pairs");
    }
    if(first > second) std::swap(first, second);
    if(second - first < 2){
        throw std::invalid_argument("pair_swap pairs must not overlap");
    }
    std::swap(tour[first], tour[second]);
    std::swap(tour[first + 1], tour[second + 1]);
}

inline void block_swap(
    std::vector<int>& tour,
    std::size_t first_begin,
    std::size_t first_end,
    std::size_t second_begin,
    std::size_t second_end
) {
    exchange_segments(
        tour, first_begin, first_end, second_begin, second_end
    );
}

inline void cross_exchange(
    std::vector<int>& first_route,
    std::vector<int>& second_route,
    std::size_t first_begin,
    std::size_t first_end,
    std::size_t second_begin,
    std::size_t second_end
) {
    if(&first_route == &second_route){
        throw std::invalid_argument(
            "cross_exchange needs two distinct routes"
        );
    }
    if(first_begin > first_end || first_end > first_route.size()
       || second_begin > second_end || second_end > second_route.size()){
        throw std::out_of_range("invalid cross-exchange blocks");
    }
    using Difference = std::vector<int>::difference_type;
    std::vector<int> first_block(
        first_route.begin() + static_cast<Difference>(first_begin),
        first_route.begin() + static_cast<Difference>(first_end)
    );
    std::vector<int> second_block(
        second_route.begin() + static_cast<Difference>(second_begin),
        second_route.begin() + static_cast<Difference>(second_end)
    );
    first_route.erase(
        first_route.begin() + static_cast<Difference>(first_begin),
        first_route.begin() + static_cast<Difference>(first_end)
    );
    second_route.erase(
        second_route.begin() + static_cast<Difference>(second_begin),
        second_route.begin() + static_cast<Difference>(second_end)
    );
    first_route.insert(
        first_route.begin() + static_cast<Difference>(first_begin),
        second_block.begin(), second_block.end()
    );
    second_route.insert(
        second_route.begin() + static_cast<Difference>(second_begin),
        first_block.begin(), first_block.end()
    );
}

inline void cyclic_exchange(
    std::vector<std::vector<int>>& routes,
    const std::vector<std::size_t>& positions
) {
    if(routes.size() != positions.size() || routes.size() < 2){
        throw std::invalid_argument(
            "cyclic_exchange needs one valid position per route"
        );
    }
    std::vector<int> removed(routes.size());
    for(std::size_t index = 0; index < routes.size(); ++index){
        if(positions[index] >= routes[index].size()){
            throw std::out_of_range("invalid cyclic-exchange position");
        }
        removed[index] = routes[index][positions[index]];
    }
    for(std::size_t index = 0; index < routes.size(); ++index){
        routes[(index + 1) % routes.size()][positions[(index + 1) % routes.size()]]
            = removed[index];
    }
}

inline void apply_k_opt(
    std::vector<int>& tour,
    const std::vector<std::size_t>& cuts,
    const std::vector<std::size_t>& segment_order,
    const std::vector<unsigned char>& reverse_segment_flags
) {
    if(cuts.size() < 2 || cuts.front() != 0 || cuts.back() != tour.size()){
        throw std::invalid_argument("k-opt cuts must start at 0 and end at n");
    }
    const std::size_t segment_count = cuts.size() - 1;
    if(segment_order.size() != segment_count
       || reverse_segment_flags.size() != segment_count){
        throw std::invalid_argument("k-opt segment arrays have wrong size");
    }
    for(std::size_t index = 0; index < segment_count; ++index){
        if(cuts[index] > cuts[index + 1]){
            throw std::invalid_argument("k-opt cuts must be nondecreasing");
        }
    }
    std::vector<unsigned char> used(segment_count, 0);
    std::vector<int> result;
    result.reserve(tour.size());
    for(std::size_t output = 0; output < segment_count; ++output){
        const std::size_t segment = segment_order[output];
        if(segment >= segment_count || used[segment]){
            throw std::invalid_argument(
                "k-opt segment_order must be a permutation"
            );
        }
        used[segment] = 1;
        using Difference = std::vector<int>::difference_type;
        auto first = tour.begin() + static_cast<Difference>(cuts[segment]);
        auto last = tour.begin() + static_cast<Difference>(cuts[segment + 1]);
        if(reverse_segment_flags[output]){
            result.insert(result.end(), std::make_reverse_iterator(last),
                          std::make_reverse_iterator(first));
        }else{
            result.insert(result.end(), first, last);
        }
    }
    tour = std::move(result);
}

inline void apply_double_bridge(
    std::vector<int>& tour,
    std::size_t first_cut,
    std::size_t second_cut,
    std::size_t third_cut,
    std::size_t fourth_cut
) {
    if(!(first_cut < second_cut && second_cut < third_cut
         && third_cut < fourth_cut && fourth_cut <= tour.size())){
        throw std::out_of_range(
            "double-bridge cuts must satisfy a < b < c < d <= n"
        );
    }
    apply_k_opt(
        tour,
        {0, first_cut, second_cut, third_cut, fourth_cut, tour.size()},
        {0, 3, 2, 1, 4},
        {0, 0, 0, 0, 0}
    );
}

inline void apply_four_opt(
    std::vector<int>& tour,
    std::size_t first_cut,
    std::size_t second_cut,
    std::size_t third_cut,
    std::size_t fourth_cut
) {
    apply_double_bridge(
        tour, first_cut, second_cut, third_cut, fourth_cut
    );
}

struct EjectionMove {
    std::size_t from_route = 0;
    std::size_t from_position = 0;
    std::size_t to_route = 0;
    std::size_t insert_before = 0;
};

inline void apply_ejection_chain(
    std::vector<std::vector<int>>& routes,
    const std::vector<EjectionMove>& moves
) {
    for(const EjectionMove& move : moves){
        if(move.from_route >= routes.size() || move.to_route >= routes.size()){
            throw std::out_of_range("invalid ejection-chain route");
        }
        auto& source = routes[move.from_route];
        auto& destination = routes[move.to_route];
        if(move.from_position >= source.size()
           || move.insert_before > destination.size()){
            throw std::out_of_range("invalid ejection-chain position");
        }
        const int vertex = source[move.from_position];
        source.erase(
            source.begin()
            + static_cast<std::vector<int>::difference_type>(
                move.from_position
            )
        );
        std::size_t position = move.insert_before;
        if(move.from_route == move.to_route
           && move.from_position < move.insert_before){
            --position;
        }
        destination.insert(
            destination.begin()
            + static_cast<std::vector<int>::difference_type>(position),
            vertex
        );
    }
}

template<class State, class Score, class Compare = std::less<Score>>
class EjectionPool {
    struct Entry {
        Score score;
        std::size_t order;
        State state;
    };
    struct Worse {
        Compare compare;
        bool operator()(const Entry& first, const Entry& second) const {
            if(compare(first.score, second.score)) return true;
            if(compare(second.score, first.score)) return false;
            return first.order < second.order;
        }
    };

    std::size_t capacity_;
    std::size_t next_order_ = 0;
    Compare compare_;
    std::priority_queue<Entry, std::vector<Entry>, Worse> entries_;

public:
    explicit EjectionPool(
        std::size_t capacity,
        Compare compare = {}
    )
        : capacity_(capacity),
          compare_(std::move(compare)),
          entries_(Worse{compare_}) {
        if(capacity_ == 0){
            throw std::invalid_argument("EjectionPool capacity must be positive");
        }
    }

    void push(State state, Score score) {
        entries_.push(Entry{std::move(score), next_order_++, std::move(state)});
        if(entries_.size() > capacity_) entries_.pop();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return entries_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return entries_.empty();
    }

    [[nodiscard]] std::vector<std::pair<State, Score>> sorted() const {
        auto copy = entries_;
        std::vector<Entry> temporary;
        while(!copy.empty()){
            temporary.push_back(copy.top());
            copy.pop();
        }
        std::sort(
            temporary.begin(), temporary.end(),
            [this](const Entry& first, const Entry& second){
                if(compare_(first.score, second.score)) return true;
                if(compare_(second.score, first.score)) return false;
                return first.order < second.order;
            }
        );
        std::vector<std::pair<State, Score>> result;
        result.reserve(temporary.size());
        for(const Entry& entry : temporary){
            result.emplace_back(entry.state, entry.score);
        }
        return result;
    }
};

inline std::vector<std::vector<int>> path_relinking(
    std::vector<int> current,
    const std::vector<int>& target,
    std::size_t maximum_steps = std::numeric_limits<std::size_t>::max()
) {
    if(current.size() != target.size()){
        throw std::invalid_argument("path_relinking tours have different sizes");
    }
    std::vector<std::size_t> position(current.size());
    std::vector<unsigned char> seen(current.size(), 0);
    for(std::size_t index = 0; index < current.size(); ++index){
        const int vertex = current[index];
        if(vertex < 0 || static_cast<std::size_t>(vertex) >= current.size()
           || seen[static_cast<std::size_t>(vertex)]){
            throw std::invalid_argument(
                "path_relinking tours must be permutations of [0,n)"
            );
        }
        seen[static_cast<std::size_t>(vertex)] = 1;
        position[static_cast<std::size_t>(vertex)] = index;
    }
    std::fill(seen.begin(), seen.end(), 0);
    for(const int vertex : target){
        if(vertex < 0 || static_cast<std::size_t>(vertex) >= target.size()
           || seen[static_cast<std::size_t>(vertex)]){
            throw std::invalid_argument(
                "path_relinking tours must be permutations of [0,n)"
            );
        }
        seen[static_cast<std::size_t>(vertex)] = 1;
    }

    std::vector<std::vector<int>> path;
    path.push_back(current);
    for(std::size_t index = 0;
        index < current.size() && path.size() - 1 < maximum_steps;
        ++index){
        if(current[index] == target[index]) continue;
        const std::size_t other =
            position[static_cast<std::size_t>(target[index])];
        position[static_cast<std::size_t>(current[index])] = other;
        std::swap(current[index], current[other]);
        position[static_cast<std::size_t>(current[index])] = index;
        path.push_back(current);
    }
    return path;
}

template<class DistanceMatrix>
std::vector<int> lin_kernighan_tour(
    const DistanceMatrix& distance,
    std::vector<int> tour,
    std::size_t maximum_passes = 100
) {
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_tour_vertices(tour, n);
    if(tour.size() != n){
        throw std::invalid_argument(
            "lin_kernighan_tour needs a Hamiltonian tour"
        );
    }
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        Cost best_delta{};
        std::size_t best_first = 0;
        std::size_t best_last = 0;
        for(std::size_t first = 0; first < n; ++first){
            for(std::size_t last = first + 2; last <= n; ++last){
                if(first == 0 && last == n) continue;
                const Cost delta =
                    two_opt_symmetric_delta(distance, tour, first, last);
                if(delta < best_delta){
                    best_delta = delta;
                    best_first = first;
                    best_last = last;
                }
            }
        }
        if(!(best_delta < Cost{})) break;
        apply_two_opt(tour, best_first, best_last);
    }
    return tour;
}

template<class DistanceMatrix>
std::vector<int> lin_kernighan_helsgaun_tour(
    const DistanceMatrix& distance,
    std::vector<int> tour,
    std::size_t candidate_count = 20,
    std::size_t maximum_passes = 100
) {
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_tour_vertices(tour, n);
    if(tour.size() != n){
        throw std::invalid_argument(
            "lin_kernighan_helsgaun_tour needs a Hamiltonian tour"
        );
    }
    if(n < 4 || candidate_count >= n){
        return lin_kernighan_tour(
            distance, std::move(tour), maximum_passes
        );
    }

    std::vector<std::vector<int>> candidates(n);
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        std::vector<int> order;
        order.reserve(n - 1);
        for(std::size_t other = 0; other < n; ++other){
            if(other != vertex) order.push_back(static_cast<int>(other));
        }
        const std::size_t keep = std::min(candidate_count, order.size());
        std::partial_sort(
            order.begin(),
            order.begin()
                + static_cast<std::vector<int>::difference_type>(keep),
            order.end(),
            [&](int first, int second){
                const auto first_cost = distance[vertex][
                    static_cast<std::size_t>(first)
                ];
                const auto second_cost = distance[vertex][
                    static_cast<std::size_t>(second)
                ];
                return first_cost < second_cost
                    || (first_cost == second_cost && first < second);
            }
        );
        order.resize(keep);
        candidates[vertex] = std::move(order);
    }

    std::vector<std::size_t> position(n);
    for(std::size_t index = 0; index < n; ++index){
        position[static_cast<std::size_t>(tour[index])] = index;
    }
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        Cost best_delta{};
        std::size_t best_first = 0;
        std::size_t best_last = 0;
        for(std::size_t first = 0; first < n; ++first){
            const std::size_t previous = (first + n - 1) % n;
            const std::size_t left =
                static_cast<std::size_t>(tour[previous]);
            for(const int candidate_vertex : candidates[left]){
                std::size_t last =
                    position[static_cast<std::size_t>(candidate_vertex)] + 1;
                if(last <= first + 1 || (first == 0 && last == n)) continue;
                const Cost delta =
                    two_opt_symmetric_delta(distance, tour, first, last);
                if(delta < best_delta){
                    best_delta = delta;
                    best_first = first;
                    best_last = last;
                }
            }
        }
        if(!(best_delta < Cost{})) break;
        apply_two_opt(tour, best_first, best_last);
        for(std::size_t index = best_first; index < best_last; ++index){
            position[static_cast<std::size_t>(tour[index])] = index;
        }
    }
    return tour;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_ADVANCED_LOCAL_SEARCH_HPP_INCLUDED
