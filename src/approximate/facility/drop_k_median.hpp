#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_DROP_K_MEDIAN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_DROP_K_MEDIAN_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::facility {

template<class T>
KMedianResult drop_k_median(const std::vector<std::vector<T>>& distance, std::size_t k) {
    const auto checked = detail::checked_distance_matrix(distance);
    detail::validate_k(checked.size(), k);
    std::vector<std::size_t> facilities(checked.size());
    for(std::size_t facility = 0; facility < facilities.size(); ++facility){
        facilities[facility] = facility;
    }
    const std::size_t client_count = checked.front().size();
    while(facilities.size() > k){
        std::vector<std::size_t> nearest_facility(client_count);
        std::vector<long double> nearest_distance(
            client_count, std::numeric_limits<long double>::infinity()
        );
        std::vector<long double> second_distance(
            client_count, std::numeric_limits<long double>::infinity()
        );
        for(std::size_t client = 0; client < client_count; ++client){
            for(std::size_t facility: facilities){
                const long double candidate = checked[facility][client];
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
        std::size_t best_removal = facilities.front();
        long double best_cost = std::numeric_limits<long double>::infinity();
        for(std::size_t facility: facilities){
            long double candidate_cost = 0.0L;
            for(std::size_t client = 0; client < client_count; ++client){
                candidate_cost = detail::add_cost(
                    candidate_cost,
                    nearest_facility[client] == facility
                        ? second_distance[client] : nearest_distance[client]
                );
            }
            if(candidate_cost < best_cost
               || (candidate_cost == best_cost && facility > best_removal)){
                best_cost = candidate_cost;
                best_removal = facility;
            }
        }
        facilities.erase(std::lower_bound(facilities.begin(), facilities.end(), best_removal));
    }
    return detail::evaluate_k_median(checked, std::move(facilities));
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_DROP_K_MEDIAN_HPP_INCLUDED
