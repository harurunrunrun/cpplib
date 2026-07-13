#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::facility {

namespace detail {

inline KMedianResult add_k_median_checked(
    const std::vector<std::vector<long double>>& checked,
    std::size_t k
) {
    validate_k(checked.size(), k);
    const std::size_t client_count = checked.front().size();
    std::vector<std::size_t> facilities;
    facilities.reserve(k);
    std::vector<long double> current_distance(
        client_count, std::numeric_limits<long double>::infinity()
    );
    std::vector<unsigned char> selected(checked.size(), 0U);
    while(facilities.size() < k){
        std::size_t best_facility = checked.size();
        long double best_cost = std::numeric_limits<long double>::infinity();
        for(std::size_t facility = 0; facility < checked.size(); ++facility){
            if(selected[facility] != 0U) continue;
            long double candidate_cost = 0.0L;
            for(std::size_t client = 0; client < client_count; ++client){
                candidate_cost = add_cost(
                    candidate_cost,
                    std::min(current_distance[client], checked[facility][client])
                );
            }
            if(candidate_cost < best_cost
               || (candidate_cost == best_cost && facility < best_facility)){
                best_cost = candidate_cost;
                best_facility = facility;
            }
        }
        selected[best_facility] = 1U;
        facilities.push_back(best_facility);
        for(std::size_t client = 0; client < client_count; ++client){
            current_distance[client] = std::min(
                current_distance[client], checked[best_facility][client]
            );
        }
    }
    return evaluate_k_median(checked, std::move(facilities));
}

}  // namespace detail

template<class T>
KMedianResult add_k_median(
    const std::vector<std::vector<T>>& distance,
    std::size_t k
) {
    const auto checked = detail::checked_distance_matrix(distance);
    return detail::add_k_median_checked(checked, k);
}

}  // namespace approximate::facility
