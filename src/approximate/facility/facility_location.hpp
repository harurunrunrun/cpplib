#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_FACILITY_LOCATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_FACILITY_LOCATION_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../clustering/facility_location.hpp"
#include "common.hpp"

namespace approximate::facility {

using FacilityLocationResult = approximate::clustering::FacilityLocationResult;

namespace detail {

template<class T>
std::vector<long double> checked_opening_costs(
    const std::vector<T>& opening_cost,
    std::size_t facility_count
) {
    static_assert(std::is_arithmetic_v<T>, "opening costs must be arithmetic");
    if(opening_cost.size() != facility_count){
        throw std::invalid_argument("one opening cost is required per facility");
    }
    std::vector<long double> result(facility_count);
    for(std::size_t facility = 0; facility < facility_count; ++facility){
        result[facility] = static_cast<long double>(opening_cost[facility]);
        if(!std::isfinite(result[facility]) || result[facility] < 0.0L){
            throw std::invalid_argument("opening costs must be finite and nonnegative");
        }
    }
    return result;
}

}  // namespace detail

template<class DistanceT, class CostT>
FacilityLocationResult greedy_facility_location(
    const std::vector<std::vector<DistanceT>>& distance,
    const std::vector<CostT>& opening_cost
) {
    const std::size_t client_count = detail::validate_distance_matrix_shape(distance);
    const auto checked_opening = detail::checked_opening_costs(
        opening_cost, distance.size()
    );
    return approximate::clustering::greedy_facility_location(
        distance.size(), client_count, checked_opening,
        [&distance](std::size_t facility, std::size_t client) {
            return distance[facility][client];
        }
    );
}

template<class DistanceT, class CostT>
FacilityLocationResult add_drop_facility_location(
    const std::vector<std::vector<DistanceT>>& distance,
    const std::vector<CostT>& opening_cost,
    std::size_t maximum_iterations = 100
) {
    const std::size_t client_count = detail::validate_distance_matrix_shape(distance);
    const auto checked_opening = detail::checked_opening_costs(
        opening_cost, distance.size()
    );
    return approximate::clustering::facility_add_drop_local_search(
        distance.size(), client_count, checked_opening,
        [&distance](std::size_t facility, std::size_t client) {
            return distance[facility][client];
        },
        maximum_iterations
    );
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_FACILITY_LOCATION_HPP_INCLUDED
