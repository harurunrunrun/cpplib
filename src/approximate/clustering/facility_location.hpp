#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_FACILITY_LOCATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_FACILITY_LOCATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

struct FacilityLocationResult {
    std::vector<std::size_t> facilities;
    // labels[c] is a position in facilities.
    std::vector<std::size_t> labels;
    long double cost = 0.0L;
    std::size_t local_search_iterations = 0;
};

namespace detail {

inline FacilityLocationResult evaluate_facilities(
    const std::vector<std::vector<long double>>& distance,
    const std::vector<long double>& opening_cost,
    std::vector<std::size_t> facilities
){
    std::sort(facilities.begin(), facilities.end());
    FacilityLocationResult result;
    result.facilities = std::move(facilities);
    for(const std::size_t facility: result.facilities){
        result.cost = checked_cost_sum(
            result.cost, opening_cost[facility], "facility-location cost overflowed"
        );
    }
    if(distance.empty() || distance.front().empty()) return result;
    result.labels.resize(distance.front().size());
    for(std::size_t client = 0; client < distance.front().size(); ++client){
        std::size_t best_position = 0;
        long double best_distance = distance[result.facilities[0]][client];
        for(std::size_t position = 1; position < result.facilities.size(); ++position){
            const long double candidate = distance[result.facilities[position]][client];
            if(candidate < best_distance ||
               (candidate == best_distance &&
                result.facilities[position] < result.facilities[best_position])){
                best_position = position;
                best_distance = candidate;
            }
        }
        result.labels[client] = best_position;
        result.cost = checked_cost_sum(
            result.cost, best_distance, "facility-location cost overflowed"
        );
    }
    return result;
}

template<class Distance>
std::vector<std::vector<long double>> facility_distance_matrix(
    std::size_t facility_count,
    std::size_t client_count,
    Distance& distance
){
    std::vector<std::vector<long double>> matrix(
        facility_count, std::vector<long double>(client_count)
    );
    for(std::size_t facility = 0; facility < facility_count; ++facility){
        for(std::size_t client = 0; client < client_count; ++client){
            matrix[facility][client] = checked_distance(static_cast<long double>(
                std::invoke(distance, facility, client)
            ));
        }
    }
    return matrix;
}

inline void validate_opening_costs(const std::vector<long double>& opening_cost){
    if(opening_cost.empty()){
        throw std::invalid_argument("at least one candidate facility is required");
    }
    for(const long double cost: opening_cost){
        if(!std::isfinite(cost) || cost < 0.0L){
            throw std::invalid_argument(
                "facility opening costs must be finite and nonnegative"
            );
        }
    }
}

}  // namespace detail

template<class Distance>
[[nodiscard]] FacilityLocationResult greedy_facility_location(
    std::size_t facility_count,
    std::size_t client_count,
    const std::vector<long double>& opening_cost,
    Distance distance
){
    if(opening_cost.size() != facility_count){
        throw std::invalid_argument("one opening cost is required per facility");
    }
    detail::validate_opening_costs(opening_cost);
    const auto matrix = detail::facility_distance_matrix(
        facility_count, client_count, distance
    );
    if(client_count == 0) return {};
    FacilityLocationResult current;
    current.cost = std::numeric_limits<long double>::infinity();
    for(std::size_t facility = 0; facility < facility_count; ++facility){
        auto trial = detail::evaluate_facilities(matrix, opening_cost, {facility});
        if(trial.cost < current.cost ||
           (trial.cost == current.cost && trial.facilities < current.facilities)){
            current = std::move(trial);
        }
    }
    while(true){
        FacilityLocationResult best = current;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            if(std::binary_search(
                current.facilities.begin(), current.facilities.end(), facility
            )) continue;
            std::vector<std::size_t> trial_facilities = current.facilities;
            trial_facilities.push_back(facility);
            auto trial = detail::evaluate_facilities(
                matrix, opening_cost, std::move(trial_facilities)
            );
            if(trial.cost < best.cost ||
               (trial.cost == best.cost && trial.cost < current.cost &&
                trial.facilities < best.facilities)){
                best = std::move(trial);
            }
        }
        if(best.cost >= current.cost) break;
        current = std::move(best);
    }
    return current;
}

template<class Distance>
[[nodiscard]] FacilityLocationResult facility_add_drop_local_search(
    std::size_t facility_count,
    std::size_t client_count,
    const std::vector<long double>& opening_cost,
    Distance distance,
    std::size_t maximum_iterations = 100
){
    if(opening_cost.size() != facility_count){
        throw std::invalid_argument("one opening cost is required per facility");
    }
    detail::validate_opening_costs(opening_cost);
    const auto matrix = detail::facility_distance_matrix(
        facility_count, client_count, distance
    );
    if(client_count == 0) return {};
    auto current = greedy_facility_location(
        facility_count,
        client_count,
        opening_cost,
        [&matrix](std::size_t facility, std::size_t client){
            return matrix[facility][client];
        }
    );
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        FacilityLocationResult best = current;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            if(std::binary_search(
                current.facilities.begin(), current.facilities.end(), facility
            )) continue;
            auto candidates = current.facilities;
            candidates.push_back(facility);
            auto trial = detail::evaluate_facilities(
                matrix, opening_cost, std::move(candidates)
            );
            if(trial.cost < best.cost ||
               (trial.cost == best.cost && trial.cost < current.cost &&
                trial.facilities < best.facilities)){
                best = std::move(trial);
            }
        }
        if(current.facilities.size() > 1){
            for(std::size_t position = 0; position < current.facilities.size(); ++position){
                auto candidates = current.facilities;
                candidates.erase(
                    candidates.begin() + static_cast<std::ptrdiff_t>(position)
                );
                auto trial = detail::evaluate_facilities(
                    matrix, opening_cost, std::move(candidates)
                );
                if(trial.cost < best.cost ||
                   (trial.cost == best.cost && trial.cost < current.cost &&
                    trial.facilities < best.facilities)){
                    best = std::move(trial);
                }
            }
        }
        if(best.cost >= current.cost) break;
        best.local_search_iterations = current.local_search_iterations + 1;
        current = std::move(best);
    }
    return current;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_FACILITY_LOCATION_HPP_INCLUDED
