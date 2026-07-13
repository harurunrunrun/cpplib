#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::facility {

namespace detail {

inline KMedianResult teitz_bart_checked(
    const std::vector<std::vector<long double>>& distance,
    std::vector<std::size_t> initial_facilities,
    std::size_t maximum_iterations
) {
    validate_facilities(distance.size(), initial_facilities);
    std::sort(initial_facilities.begin(), initial_facilities.end());
    KMedianResult current = evaluate_k_median(distance, std::move(initial_facilities));
    const std::size_t client_count = distance.front().size();
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        std::vector<unsigned char> selected(distance.size(), 0U);
        for(std::size_t facility: current.facilities) selected[facility] = 1U;
        std::vector<std::size_t> nearest_facility(client_count);
        std::vector<long double> nearest_distance(
            client_count, std::numeric_limits<long double>::infinity()
        );
        std::vector<long double> second_distance(
            client_count, std::numeric_limits<long double>::infinity()
        );
        for(std::size_t client = 0; client < client_count; ++client){
            for(std::size_t facility: current.facilities){
                const long double candidate = distance[facility][client];
                if(candidate < nearest_distance[client]
                   || (candidate == nearest_distance[client]
                       && facility < nearest_facility[client])){
                    second_distance[client] = nearest_distance[client];
                    nearest_distance[client] = candidate;
                    nearest_facility[client] = facility;
                }else if(candidate < second_distance[client]){
                    second_distance[client] = candidate;
                }
            }
        }
        long double best_cost = current.cost;
        std::vector<std::size_t> best_facilities = current.facilities;
        for(std::size_t removed: current.facilities){
            for(std::size_t added = 0; added < distance.size(); ++added){
                if(selected[added] != 0U) continue;
                long double candidate_cost = 0.0L;
                for(std::size_t client = 0; client < client_count; ++client){
                    const long double without_removed = nearest_facility[client] == removed
                        ? second_distance[client] : nearest_distance[client];
                    candidate_cost = add_cost(
                        candidate_cost, std::min(without_removed, distance[added][client])
                    );
                }
                if(candidate_cost > best_cost || candidate_cost >= current.cost) continue;
                std::vector<std::size_t> candidate_facilities = current.facilities;
                *std::lower_bound(
                    candidate_facilities.begin(), candidate_facilities.end(), removed
                ) = added;
                std::sort(candidate_facilities.begin(), candidate_facilities.end());
                if(candidate_cost < best_cost || candidate_facilities < best_facilities){
                    best_cost = candidate_cost;
                    best_facilities = std::move(candidate_facilities);
                }
            }
        }
        if(best_cost >= current.cost) break;
        KMedianResult next = evaluate_k_median(distance, std::move(best_facilities));
        next.local_search_iterations = current.local_search_iterations + 1;
        current = std::move(next);
    }
    return current;
}

}  // namespace detail

template<class T>
KMedianResult teitz_bart_k_median(
    const std::vector<std::vector<T>>& distance,
    std::vector<std::size_t> initial_facilities,
    std::size_t maximum_iterations = 100
) {
    const auto checked = detail::checked_distance_matrix(distance);
    return detail::teitz_bart_checked(
        checked, std::move(initial_facilities), maximum_iterations
    );
}

}  // namespace approximate::facility
