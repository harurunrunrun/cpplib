#pragma once

#include <cstddef>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

template<class DistanceMatrix>
auto tour_cost(
    const DistanceMatrix& distance,
    const std::vector<int>& tour
) -> internal::distance_cost_t<DistanceMatrix> {
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_tour_vertices(tour, n);
    if(tour.empty()) return Cost{};

    Cost result{};
    for(std::size_t i = 0; i < tour.size(); ++i){
        const std::size_t from = static_cast<std::size_t>(tour[i]);
        const std::size_t to = static_cast<std::size_t>(
            tour[(i + 1) % tour.size()]
        );
        result += distance[from][to];
    }
    return result;
}

template<class DistanceMatrix>
auto path_cost(
    const DistanceMatrix& distance,
    const std::vector<int>& path
) -> internal::distance_cost_t<DistanceMatrix> {
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_tour_vertices(path, n);

    Cost result{};
    for(std::size_t i = 1; i < path.size(); ++i){
        const std::size_t from = static_cast<std::size_t>(path[i - 1]);
        const std::size_t to = static_cast<std::size_t>(path[i]);
        result += distance[from][to];
    }
    return result;
}

}  // namespace approximate::routing
