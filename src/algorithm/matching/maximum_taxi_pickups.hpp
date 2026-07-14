#pragma once

#include <cstdlib>
#include <stdexcept>
#include <utility>
#include <vector>

#include "maximum_bipartite_pair_count.hpp"

inline int maximum_taxi_pickups(
    const std::vector<std::pair<int, int>>& passenger_positions,
    const std::vector<std::pair<int, int>>& taxi_positions,
    long long speed_meters_per_second,
    long long pickup_time_seconds,
    long long block_length_meters = 200
){
    if(speed_meters_per_second < 0 || pickup_time_seconds < 0 ||
       block_length_meters <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (maximum_taxi_pickups)."
        );
    }
    const __int128 distance_limit =
        static_cast<__int128>(speed_meters_per_second) * pickup_time_seconds;
    std::vector<std::pair<int, int>> reachable_pairs;
    for(int passenger = 0;
        passenger < static_cast<int>(passenger_positions.size()); passenger++){
        const auto [passenger_x, passenger_y] =
            passenger_positions[static_cast<std::size_t>(passenger)];
        for(int taxi = 0; taxi < static_cast<int>(taxi_positions.size()); taxi++){
            const auto [taxi_x, taxi_y] =
                taxi_positions[static_cast<std::size_t>(taxi)];
            const long long dx = std::llabs(
                static_cast<long long>(passenger_x) - taxi_x
            );
            const long long dy = std::llabs(
                static_cast<long long>(passenger_y) - taxi_y
            );
            const __int128 distance =
                static_cast<__int128>(dx + dy) * block_length_meters;
            if(distance <= distance_limit){
                reachable_pairs.emplace_back(passenger, taxi);
            }
        }
    }
    return maximum_bipartite_pair_count(
        static_cast<int>(passenger_positions.size()),
        static_cast<int>(taxi_positions.size()),
        reachable_pairs
    );
}
