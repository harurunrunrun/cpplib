#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_COMMON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_COMMON_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../clustering/common.hpp"

namespace approximate::facility {

struct KMedianResult {
    std::vector<std::size_t> facilities;
    std::vector<std::size_t> assignment;
    long double cost = 0.0L;
    std::size_t local_search_iterations = 0;
};

namespace detail {

template<class T>
std::size_t validate_distance_matrix_shape(
    const std::vector<std::vector<T>>& distance
) {
    static_assert(std::is_arithmetic_v<T>, "distance entries must be arithmetic");
    if(distance.empty()){
        throw std::invalid_argument("at least one candidate facility is required");
    }
    const std::size_t client_count = distance.front().size();
    for(const auto& row: distance){
        if(row.size() != client_count){
            throw std::invalid_argument("distance matrix must be rectangular");
        }
    }
    return client_count;
}

template<class T>
std::vector<std::vector<long double>> checked_distance_matrix(
    const std::vector<std::vector<T>>& distance
) {
    const std::size_t client_count = validate_distance_matrix_shape(distance);
    std::vector<std::vector<long double>> result(
        distance.size(), std::vector<long double>(client_count)
    );
    for(std::size_t facility = 0; facility < distance.size(); ++facility){
        for(std::size_t client = 0; client < client_count; ++client){
            result[facility][client] = approximate::clustering::detail::checked_distance(
                static_cast<long double>(distance[facility][client])
            );
        }
    }
    return result;
}

inline void validate_k(std::size_t facility_count, std::size_t k) {
    if(k == 0 || k > facility_count){
        throw std::invalid_argument("k must be in [1, facility count]");
    }
}

inline void validate_facilities(
    std::size_t facility_count,
    const std::vector<std::size_t>& facilities
) {
    if(facilities.empty()) throw std::invalid_argument("facility set must be nonempty");
    std::vector<std::size_t> sorted = facilities;
    std::sort(sorted.begin(), sorted.end());
    if(sorted.back() >= facility_count){
        throw std::invalid_argument("facility index is out of range");
    }
    if(std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end()){
        throw std::invalid_argument("facility indices must be distinct");
    }
}

inline long double add_cost(long double total, long double value) {
    return approximate::clustering::detail::checked_cost_sum(
        total, value, "facility objective overflowed"
    );
}

inline KMedianResult evaluate_k_median(
    const std::vector<std::vector<long double>>& distance,
    std::vector<std::size_t> facilities
) {
    validate_facilities(distance.size(), facilities);
    std::sort(facilities.begin(), facilities.end());
    KMedianResult result;
    result.facilities = std::move(facilities);
    const std::size_t client_count = distance.front().size();
    result.assignment.resize(client_count);
    for(std::size_t client = 0; client < client_count; ++client){
        std::size_t best_facility = result.facilities.front();
        long double best_distance = distance[best_facility][client];
        for(std::size_t position = 1; position < result.facilities.size(); ++position){
            const std::size_t facility = result.facilities[position];
            const long double candidate = distance[facility][client];
            if(candidate < best_distance
               || (candidate == best_distance && facility < best_facility)){
                best_distance = candidate;
                best_facility = facility;
            }
        }
        result.assignment[client] = best_facility;
        result.cost = add_cost(result.cost, best_distance);
    }
    return result;
}

inline bool better_result(const KMedianResult& candidate, const KMedianResult& current) {
    return candidate.cost < current.cost
        || (candidate.cost == current.cost && candidate.facilities < current.facilities);
}

}  // namespace detail

template<class T>
KMedianResult evaluate_k_median(
    const std::vector<std::vector<T>>& distance,
    std::vector<std::size_t> facilities
) {
    const auto checked = detail::checked_distance_matrix(distance);
    return detail::evaluate_k_median(checked, std::move(facilities));
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_COMMON_HPP_INCLUDED
