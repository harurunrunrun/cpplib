#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_PRIMAL_DUAL_FACILITY_LOCATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_PRIMAL_DUAL_FACILITY_LOCATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../clustering/facility_location.hpp"
#include "common.hpp"

namespace approximate::facility {

struct PrimalDualFacilityLocationResult {
    approximate::clustering::FacilityLocationResult solution;
    std::vector<long double> client_dual;
    std::vector<std::size_t> tight_facilities;
};

namespace primal_dual_facility_internal {

inline long double time_until_tight(
    std::size_t facility,
    const std::vector<std::vector<long double>>& distance,
    const std::vector<long double>& opening_cost,
    const std::vector<long double>& dual,
    const std::vector<unsigned char>& active
) {
    long double contribution = 0.0L;
    std::vector<long double> breakpoints;
    breakpoints.reserve(dual.size());
    std::size_t slope = 0;
    for(std::size_t client = 0; client < dual.size(); ++client){
        if(active[client] == 0){
            contribution += std::max(
                0.0L, dual[client] - distance[facility][client]
            );
            continue;
        }
        const long double breakpoint =
            std::max(0.0L, distance[facility][client] - dual[client]);
        if(breakpoint == 0.0L) ++slope;
        else breakpoints.push_back(breakpoint);
    }
    long double slack = opening_cost[facility] - contribution;
    const long double scale = std::max(1.0L, opening_cost[facility]);
    if(slack <= 32.0L * std::numeric_limits<long double>::epsilon() * scale){
        return 0.0L;
    }
    std::sort(breakpoints.begin(), breakpoints.end());
    long double previous = 0.0L;
    std::size_t position = 0;
    while(position < breakpoints.size()){
        const long double breakpoint = breakpoints[position];
        if(slope != 0){
            const long double gain =
                static_cast<long double>(slope) * (breakpoint - previous);
            if(slack <= gain){
                return previous + slack / static_cast<long double>(slope);
            }
            slack -= gain;
        }
        while(position < breakpoints.size()
              && breakpoints[position] == breakpoint){
            ++slope;
            ++position;
        }
        previous = breakpoint;
    }
    if(slope == 0) return std::numeric_limits<long double>::infinity();
    return previous + slack / static_cast<long double>(slope);
}

}  // namespace primal_dual_facility_internal

template<class Distance>
[[nodiscard]] PrimalDualFacilityLocationResult
primal_dual_facility_location(
    std::size_t facility_count,
    std::size_t client_count,
    const std::vector<long double>& opening_cost,
    Distance distance
) {
    using namespace primal_dual_facility_internal;
    if(opening_cost.size() != facility_count){
        throw std::invalid_argument(
            "one opening cost is required per facility"
        );
    }
    approximate::clustering::detail::validate_opening_costs(opening_cost);
    const auto matrix =
        approximate::clustering::detail::facility_distance_matrix(
            facility_count, client_count, distance
        );
    PrimalDualFacilityLocationResult result;
    result.client_dual.assign(client_count, 0.0L);
    if(client_count == 0) return result;

    std::vector<unsigned char> active(client_count, 1);
    std::vector<unsigned char> is_tight(facility_count, 0);
    std::size_t active_count = client_count;
    while(active_count != 0){
        long double delta = std::numeric_limits<long double>::infinity();
        std::size_t event_facility = facility_count;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            long double candidate =
                std::numeric_limits<long double>::infinity();
            if(is_tight[facility] == 0){
                candidate = time_until_tight(
                    facility, matrix, opening_cost,
                    result.client_dual, active
                );
            }else{
                for(std::size_t client = 0; client < client_count; ++client){
                    if(active[client] == 0) continue;
                    candidate = std::min(
                        candidate,
                        std::max(
                            0.0L,
                            matrix[facility][client]
                                - result.client_dual[client]
                        )
                    );
                }
            }
            if(candidate < delta){
                delta = candidate;
                event_facility = facility;
            }
        }
        if(event_facility == facility_count || !std::isfinite(delta)){
            throw std::logic_error(
                "primal-dual facility location could not cover every client"
            );
        }
        for(std::size_t client = 0; client < client_count; ++client){
            if(active[client] != 0) result.client_dual[client] += delta;
        }
        std::vector<std::size_t> new_tight;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            if(is_tight[facility] != 0) continue;
            long double contribution = 0.0L;
            for(std::size_t client = 0; client < client_count; ++client){
                contribution += std::max(
                    0.0L,
                    result.client_dual[client] - matrix[facility][client]
                );
            }
            const long double scale = std::max(1.0L, opening_cost[facility]);
            if(opening_cost[facility] - contribution
               <= 64.0L * std::numeric_limits<long double>::epsilon()
                    * scale){
                is_tight[facility] = 1;
                result.tight_facilities.push_back(facility);
                new_tight.push_back(facility);
            }
        }
        if(new_tight.empty() && is_tight[event_facility] == 0){
            is_tight[event_facility] = 1;
            result.tight_facilities.push_back(event_facility);
            new_tight.push_back(event_facility);
        }
        bool deactivated = false;
        for(std::size_t client = 0; client < client_count; ++client){
            if(active[client] == 0) continue;
            for(std::size_t facility = 0; facility < facility_count; ++facility){
                if(is_tight[facility] == 0) continue;
                if(matrix[facility][client] <= result.client_dual[client]){
                    active[client] = 0;
                    --active_count;
                    deactivated = true;
                    break;
                }
            }
        }
        if(!deactivated && new_tight.empty()){
            throw std::logic_error(
                "primal-dual facility location made no progress"
            );
        }
    }

    std::vector<unsigned char> contributor_used(client_count, 0);
    std::vector<std::size_t> selected;
    for(auto iterator = result.tight_facilities.rbegin();
        iterator != result.tight_facilities.rend(); ++iterator){
        const std::size_t facility = *iterator;
        bool conflicts = false;
        for(std::size_t client = 0; client < client_count; ++client){
            if(contributor_used[client] != 0
               && matrix[facility][client] < result.client_dual[client]){
                conflicts = true;
                break;
            }
        }
        if(conflicts) continue;
        selected.push_back(facility);
        for(std::size_t client = 0; client < client_count; ++client){
            if(matrix[facility][client] < result.client_dual[client]){
                contributor_used[client] = 1;
            }
        }
    }
    if(selected.empty()){
        selected.push_back(result.tight_facilities.front());
    }
    result.solution =
        approximate::clustering::detail::evaluate_facilities(
            matrix, opening_cost, std::move(selected)
        );
    return result;
}

template<class DistanceT, class CostT>
[[nodiscard]] PrimalDualFacilityLocationResult
primal_dual_facility_location(
    const std::vector<std::vector<DistanceT>>& distance,
    const std::vector<CostT>& opening_cost
) {
    static_assert(
        std::is_arithmetic_v<DistanceT> && std::is_arithmetic_v<CostT>
    );
    const std::size_t client_count =
        detail::validate_distance_matrix_shape(distance);
    std::vector<long double> checked(opening_cost.size());
    for(std::size_t facility = 0; facility < opening_cost.size(); ++facility){
        checked[facility] = static_cast<long double>(opening_cost[facility]);
    }
    return primal_dual_facility_location(
        distance.size(), client_count, checked,
        [&](std::size_t facility, std::size_t client){
            return distance[facility][client];
        }
    );
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_PRIMAL_DUAL_FACILITY_LOCATION_HPP_INCLUDED
