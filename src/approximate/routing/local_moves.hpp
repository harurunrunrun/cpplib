#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

inline void reverse_segment(
    std::vector<int>& tour,
    std::size_t first,
    std::size_t last
){
    if(first > last || last > tour.size()){
        throw std::out_of_range("invalid segment");
    }
    using Difference = std::vector<int>::difference_type;
    std::reverse(
        tour.begin() + static_cast<Difference>(first),
        tour.begin() + static_cast<Difference>(last)
    );
}

inline void apply_two_opt(
    std::vector<int>& tour,
    std::size_t first,
    std::size_t last
){
    reverse_segment(tour, first, last);
}

template<class DistanceMatrix>
auto two_opt_symmetric_delta(
    const DistanceMatrix& distance,
    const std::vector<int>& tour,
    std::size_t first,
    std::size_t last
) -> internal::distance_cost_t<DistanceMatrix> {
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    if(first > last || last > tour.size()){
        throw std::out_of_range("invalid 2-opt segment");
    }
    if(tour.size() < 2 || last - first < 2 || (first == 0 && last == tour.size())){
        return Cost{};
    }

    const std::size_t previous = (first + tour.size() - 1) % tour.size();
    const std::size_t next = last % tour.size();
    const std::size_t old_left = static_cast<std::size_t>(tour[previous]);
    const std::size_t old_first = static_cast<std::size_t>(tour[first]);
    const std::size_t old_last = static_cast<std::size_t>(tour[last - 1]);
    const std::size_t old_right = static_cast<std::size_t>(tour[next]);
    return distance[old_left][old_last] + distance[old_first][old_right]
        - distance[old_left][old_first] - distance[old_last][old_right];
}

template<class DistanceMatrix>
auto two_opt_star_delta(
    const DistanceMatrix& distance,
    const std::vector<int>& first_route,
    const std::vector<int>& second_route,
    std::size_t first_cut,
    std::size_t second_cut
) -> internal::distance_cost_t<DistanceMatrix> {
    if(
        first_cut == 0 || first_cut >= first_route.size()
        || second_cut == 0 || second_cut >= second_route.size()
    ){
        throw std::out_of_range("2-opt* delta needs two internal path cuts");
    }
    const std::size_t first_previous = static_cast<std::size_t>(
        first_route[first_cut - 1]
    );
    const std::size_t first_next = static_cast<std::size_t>(first_route[first_cut]);
    const std::size_t second_previous = static_cast<std::size_t>(
        second_route[second_cut - 1]
    );
    const std::size_t second_next = static_cast<std::size_t>(second_route[second_cut]);
    return distance[first_previous][second_next]
        + distance[second_previous][first_next]
        - distance[first_previous][first_next]
        - distance[second_previous][second_next];
}

inline void apply_two_opt_star(
    std::vector<int>& first_route,
    std::vector<int>& second_route,
    std::size_t first_cut,
    std::size_t second_cut
){
    if(first_cut > first_route.size() || second_cut > second_route.size()){
        throw std::out_of_range("invalid 2-opt* cut");
    }
    using Difference = std::vector<int>::difference_type;
    std::vector<int> first_tail(
        first_route.begin() + static_cast<Difference>(first_cut),
        first_route.end()
    );
    std::vector<int> second_tail(
        second_route.begin() + static_cast<Difference>(second_cut),
        second_route.end()
    );
    first_route.erase(
        first_route.begin() + static_cast<Difference>(first_cut),
        first_route.end()
    );
    second_route.erase(
        second_route.begin() + static_cast<Difference>(second_cut),
        second_route.end()
    );
    first_route.insert(first_route.end(), second_tail.begin(), second_tail.end());
    second_route.insert(second_route.end(), first_tail.begin(), first_tail.end());
}

enum class ThreeOptPattern {
    reverse_first,
    reverse_second,
    reverse_both,
    exchange,
    reverse_second_exchange,
    exchange_reverse_first,
    reverse_both_exchange,
};

inline void apply_three_opt(
    std::vector<int>& tour,
    std::size_t first_cut,
    std::size_t second_cut,
    std::size_t third_cut,
    ThreeOptPattern pattern
){
    if(
        first_cut == 0 || first_cut >= second_cut
        || second_cut >= third_cut || third_cut >= tour.size()
    ){
        throw std::out_of_range("3-opt cuts must satisfy 0 < a < b < c < n");
    }
    using Difference = std::vector<int>::difference_type;
    std::vector<int> first(
        tour.begin() + static_cast<Difference>(first_cut),
        tour.begin() + static_cast<Difference>(second_cut)
    );
    std::vector<int> second(
        tour.begin() + static_cast<Difference>(second_cut),
        tour.begin() + static_cast<Difference>(third_cut)
    );
    const bool reverse_first = pattern == ThreeOptPattern::reverse_first
        || pattern == ThreeOptPattern::reverse_both
        || pattern == ThreeOptPattern::exchange_reverse_first
        || pattern == ThreeOptPattern::reverse_both_exchange;
    const bool reverse_second = pattern == ThreeOptPattern::reverse_second
        || pattern == ThreeOptPattern::reverse_both
        || pattern == ThreeOptPattern::reverse_second_exchange
        || pattern == ThreeOptPattern::reverse_both_exchange;
    const bool exchange = pattern == ThreeOptPattern::exchange
        || pattern == ThreeOptPattern::reverse_second_exchange
        || pattern == ThreeOptPattern::exchange_reverse_first
        || pattern == ThreeOptPattern::reverse_both_exchange;
    if(reverse_first) std::reverse(first.begin(), first.end());
    if(reverse_second) std::reverse(second.begin(), second.end());

    auto output = tour.begin() + static_cast<Difference>(first_cut);
    const auto write_segment = [&output](const std::vector<int>& segment){
        output = std::copy(segment.begin(), segment.end(), output);
    };
    if(exchange){
        write_segment(second);
        write_segment(first);
    }else{
        write_segment(first);
        write_segment(second);
    }
}

inline void relocate_segment(
    std::vector<int>& route,
    std::size_t first,
    std::size_t last,
    std::size_t insert_before
){
    if(first > last || last > route.size() || insert_before > route.size()){
        throw std::out_of_range("invalid relocation segment");
    }
    if(first == last || (first <= insert_before && insert_before <= last)) return;
    using Difference = std::vector<int>::difference_type;
    std::vector<int> block(
        route.begin() + static_cast<Difference>(first),
        route.begin() + static_cast<Difference>(last)
    );
    route.erase(
        route.begin() + static_cast<Difference>(first),
        route.begin() + static_cast<Difference>(last)
    );
    const std::size_t destination = insert_before > last
        ? insert_before - (last - first)
        : insert_before;
    route.insert(
        route.begin() + static_cast<Difference>(destination),
        block.begin(),
        block.end()
    );
}

inline void relocate_vertex(
    std::vector<int>& route,
    std::size_t vertex_position,
    std::size_t insert_before
){
    if(vertex_position == route.size()){
        throw std::out_of_range("invalid vertex position");
    }
    relocate_segment(route, vertex_position, vertex_position + 1, insert_before);
}

inline void apply_or_opt(
    std::vector<int>& route,
    std::size_t first,
    std::size_t length,
    std::size_t insert_before
){
    if(length == 0 || length > 3){
        throw std::invalid_argument("Or-opt block length must be 1, 2, or 3");
    }
    if(first > route.size() || length > route.size() - first){
        throw std::out_of_range("invalid Or-opt block");
    }
    relocate_segment(route, first, first + length, insert_before);
}

inline void swap_vertices(
    std::vector<int>& route,
    std::size_t first,
    std::size_t second
){
    if(first >= route.size() || second >= route.size()){
        throw std::out_of_range("invalid swap position");
    }
    std::swap(route[first], route[second]);
}

inline void exchange_segments(
    std::vector<int>& route,
    std::size_t first_begin,
    std::size_t first_end,
    std::size_t second_begin,
    std::size_t second_end
){
    if(first_begin > first_end || second_begin > second_end){
        throw std::out_of_range("invalid exchange segment");
    }
    if(second_begin < first_begin){
        exchange_segments(
            route,
            second_begin,
            second_end,
            first_begin,
            first_end
        );
        return;
    }
    if(first_end > second_begin || second_end > route.size()){
        throw std::out_of_range("exchange segments must not overlap");
    }
    using Difference = std::vector<int>::difference_type;
    std::vector<int> result;
    result.reserve(route.size());
    result.insert(
        result.end(),
        route.begin(),
        route.begin() + static_cast<Difference>(first_begin)
    );
    result.insert(
        result.end(),
        route.begin() + static_cast<Difference>(second_begin),
        route.begin() + static_cast<Difference>(second_end)
    );
    result.insert(
        result.end(),
        route.begin() + static_cast<Difference>(first_end),
        route.begin() + static_cast<Difference>(second_begin)
    );
    result.insert(
        result.end(),
        route.begin() + static_cast<Difference>(first_begin),
        route.begin() + static_cast<Difference>(first_end)
    );
    result.insert(
        result.end(),
        route.begin() + static_cast<Difference>(second_end),
        route.end()
    );
    route = std::move(result);
}

}  // namespace approximate::routing
