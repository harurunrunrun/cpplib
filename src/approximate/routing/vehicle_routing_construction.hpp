#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_VEHICLE_ROUTING_CONSTRUCTION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_VEHICLE_ROUTING_CONSTRUCTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

struct RoutingTimeWindow {
    long double earliest = 0;
    long double latest = std::numeric_limits<long double>::infinity();
    long double service_time = 0;
};

namespace construction_detail {

inline std::vector<int> route_with_insertion(
    const std::vector<int>& route,
    int vertex,
    std::size_t position
) {
    std::vector<int> result;
    result.reserve(route.size() + 1);
    for(std::size_t index = 0; index < position; ++index){
        result.push_back(route[index]);
    }
    result.push_back(vertex);
    for(std::size_t index = position; index < route.size(); ++index){
        result.push_back(route[index]);
    }
    return result;
}

template<class DistanceMatrix>
long double insertion_distance_delta(
    const DistanceMatrix& distance,
    const std::vector<int>& route,
    std::size_t depot,
    std::size_t vertex,
    std::size_t position
) {
    const std::size_t previous = position == 0
        ? depot : static_cast<std::size_t>(route[position - 1]);
    const std::size_t next = position == route.size()
        ? depot : static_cast<std::size_t>(route[position]);
    return static_cast<long double>(distance[previous][vertex])
         + static_cast<long double>(distance[vertex][next])
         - static_cast<long double>(distance[previous][next]);
}

template<class DistanceMatrix>
std::pair<long double, long double> route_schedule(
    const DistanceMatrix& distance,
    const std::vector<int>& route,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot
) {
    long double time = time_window[depot].earliest;
    long double waiting = 0;
    std::size_t previous = depot;
    for(const int raw_vertex : route){
        const std::size_t vertex = static_cast<std::size_t>(raw_vertex);
        time += static_cast<long double>(distance[previous][vertex]);
        if(time < time_window[vertex].earliest){
            waiting += time_window[vertex].earliest - time;
            time = time_window[vertex].earliest;
        }
        if(time > time_window[vertex].latest) {
            return {
                std::numeric_limits<long double>::infinity(),
                std::numeric_limits<long double>::infinity()
            };
        }
        time += time_window[vertex].service_time;
        previous = vertex;
    }
    time += static_cast<long double>(distance[previous][depot]);
    if(time > time_window[depot].latest){
        return {
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity()
        };
    }
    return {time, waiting};
}

template<class DistanceMatrix>
void validate_vrp_input(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double capacity,
    std::size_t depot
) {
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("vertex count does not fit in int");
    }
    if(depot >= n && n != 0) throw std::out_of_range("depot is outside matrix");
    if(n == 0) throw std::invalid_argument("VRP needs a depot");
    if(demand.size() != n){
        throw std::invalid_argument("demand has wrong size");
    }
    if(demand[depot] != 0){
        throw std::invalid_argument("depot demand must be zero");
    }
    if(!(capacity >= 0) || !std::isfinite(capacity)){
        throw std::invalid_argument("capacity must be finite and nonnegative");
    }
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(!(demand[vertex] >= 0) || !std::isfinite(demand[vertex])){
            throw std::invalid_argument(
                "every demand must be finite and nonnegative"
            );
        }
        if(vertex != depot && demand[vertex] > capacity){
            throw std::domain_error("a customer exceeds vehicle capacity");
        }
    }
}

enum class TimeInsertionRule { mole_jameson, solomon_i1, solomon_i2, solomon_i3 };

template<class DistanceMatrix>
std::vector<std::vector<int>> time_window_insertion_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double capacity,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot,
    TimeInsertionRule rule
) {
    validate_vrp_input(distance, demand, capacity, depot);
    const std::size_t n = distance.size();
    if(time_window.size() != n){
        throw std::invalid_argument("time_window has wrong size");
    }
    for(const RoutingTimeWindow& window : time_window){
        if(!std::isfinite(window.earliest)
           || !(window.latest >= window.earliest)
           || !(window.service_time >= 0)
           || !std::isfinite(window.service_time)){
            throw std::invalid_argument("invalid time window");
        }
    }
    std::vector<unsigned char> served(n, 0);
    served[depot] = 1;
    std::size_t remaining = n - 1;
    std::vector<std::vector<int>> routes;

    while(remaining != 0){
        std::size_t seed = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(served[vertex]) continue;
            std::vector<int> singleton{static_cast<int>(vertex)};
            if(!std::isfinite(
                route_schedule(distance, singleton, time_window, depot).first
            )) continue;
            if(seed == n
               || time_window[vertex].latest < time_window[seed].latest
               || (time_window[vertex].latest == time_window[seed].latest
                   && distance[depot][seed] < distance[depot][vertex])){
                seed = vertex;
            }
        }
        if(seed == n){
            throw std::domain_error(
                "an unserved customer has no feasible depot round trip"
            );
        }
        std::vector<int> route{static_cast<int>(seed)};
        long double load = demand[seed];
        served[seed] = 1;
        --remaining;

        for(;;){
            const auto old_schedule =
                route_schedule(distance, route, time_window, depot);
            std::size_t selected = n;
            std::size_t selected_position = 0;
            long double selected_primary =
                std::numeric_limits<long double>::infinity();
            long double selected_secondary =
                std::numeric_limits<long double>::infinity();
            for(std::size_t vertex = 0; vertex < n; ++vertex){
                if(served[vertex] || load + demand[vertex] > capacity) continue;
                for(std::size_t position = 0;
                    position <= route.size(); ++position){
                    const std::vector<int> candidate = route_with_insertion(
                        route, static_cast<int>(vertex), position
                    );
                    const auto schedule =
                        route_schedule(distance, candidate, time_window, depot);
                    if(!std::isfinite(schedule.first)) continue;
                    const long double distance_delta =
                        insertion_distance_delta(
                            distance, route, depot, vertex, position
                        );
                    const long double time_delta =
                        schedule.first - old_schedule.first;
                    const long double waiting_delta =
                        schedule.second - old_schedule.second;
                    long double primary = 0;
                    long double secondary = 0;
                    if(rule == TimeInsertionRule::mole_jameson){
                        const long double urgency =
                            time_window[vertex].latest
                            - time_window[vertex].earliest;
                        primary = distance_delta + time_delta
                                + 0.25L * waiting_delta;
                        secondary = urgency;
                    }else{
                        long double alpha = 1;
                        long double lambda = 1;
                        if(rule == TimeInsertionRule::solomon_i2){
                            alpha = 0;
                            lambda = 1;
                        }else if(rule == TimeInsertionRule::solomon_i3){
                            alpha = 0.5L;
                            lambda = 2;
                        }
                        const long double c1 =
                            alpha * distance_delta
                            + (1 - alpha) * time_delta;
                        primary =
                            -(lambda
                              * static_cast<long double>(
                                  distance[depot][vertex]
                              )
                              - c1);
                        secondary = c1;
                    }
                    if(primary < selected_primary
                       || (primary == selected_primary
                           && (secondary < selected_secondary
                               || (secondary == selected_secondary
                                   && (vertex < selected
                                       || (vertex == selected
                                           && position
                                               < selected_position)))))){
                        selected = vertex;
                        selected_position = position;
                        selected_primary = primary;
                        selected_secondary = secondary;
                    }
                }
            }
            if(selected == n) break;
            route.insert(
                route.begin()
                    + static_cast<std::vector<int>::difference_type>(
                        selected_position
                    ),
                static_cast<int>(selected)
            );
            load += demand[selected];
            served[selected] = 1;
            --remaining;
        }
        routes.push_back(std::move(route));
    }
    return routes;
}

}  // namespace construction_detail

template<class DistanceMatrix>
std::vector<std::vector<int>> fisher_jaikumar_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double vehicle_capacity,
    std::size_t vehicle_count,
    std::size_t depot = 0
) {
    construction_detail::validate_vrp_input(
        distance, demand, vehicle_capacity, depot
    );
    const std::size_t n = distance.size();
    if(vehicle_count == 0 && n > 1){
        throw std::invalid_argument("vehicle_count must be positive");
    }
    long double total_demand = 0;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex != depot) total_demand += demand[vertex];
    }
    if(total_demand > vehicle_capacity * vehicle_count){
        throw std::domain_error("fleet capacity is insufficient");
    }
    std::vector<std::vector<int>> routes(vehicle_count);
    if(n == 1) return routes;

    const std::size_t seed_count = std::min(vehicle_count, n - 1);
    std::vector<std::size_t> seed;
    seed.reserve(seed_count);
    std::vector<unsigned char> selected(n, 0);
    selected[depot] = 1;
    while(seed.size() < seed_count){
        std::size_t best = n;
        long double best_separation = -1;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(selected[vertex]) continue;
            long double separation =
                static_cast<long double>(distance[depot][vertex]);
            for(const std::size_t existing : seed){
                separation = std::min(
                    separation,
                    static_cast<long double>(distance[existing][vertex])
                );
            }
            if(best == n || separation > best_separation
               || (separation == best_separation && vertex < best)){
                best = vertex;
                best_separation = separation;
            }
        }
        selected[best] = 1;
        seed.push_back(best);
    }

    std::vector<long double> load(vehicle_count, 0);
    std::vector<unsigned char> assigned(n, 0);
    assigned[depot] = 1;
    for(std::size_t route = 0; route < seed_count; ++route){
        routes[route].push_back(static_cast<int>(seed[route]));
        load[route] += demand[seed[route]];
        assigned[seed[route]] = 1;
    }
    std::size_t assigned_count = 1 + seed_count;
    while(assigned_count < n){
        std::size_t customer = n;
        std::size_t best_route = vehicle_count;
        long double best_regret = -1;
        long double best_cost = 0;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(assigned[vertex]) continue;
            long double first_cost =
                std::numeric_limits<long double>::infinity();
            long double second_cost = first_cost;
            std::size_t first_route = vehicle_count;
            for(std::size_t route = 0; route < vehicle_count; ++route){
                if(load[route] + demand[vertex] > vehicle_capacity) continue;
                const std::size_t route_seed =
                    route < seed_count ? seed[route] : depot;
                const long double allocation_cost =
                    static_cast<long double>(distance[vertex][route_seed])
                    - static_cast<long double>(distance[depot][route_seed]);
                if(allocation_cost < first_cost){
                    second_cost = first_cost;
                    first_cost = allocation_cost;
                    first_route = route;
                }else if(allocation_cost < second_cost){
                    second_cost = allocation_cost;
                }
            }
            if(first_route == vehicle_count) continue;
            const long double regret = std::isfinite(second_cost)
                ? second_cost - first_cost
                : std::numeric_limits<long double>::infinity();
            if(customer == n || regret > best_regret
               || (regret == best_regret
                   && (first_cost < best_cost
                       || (first_cost == best_cost && vertex < customer)))){
                customer = vertex;
                best_route = first_route;
                best_regret = regret;
                best_cost = first_cost;
            }
        }
        if(customer == n){
            throw std::domain_error(
                "the greedy assignment cannot satisfy vehicle capacities"
            );
        }
        auto& route = routes[best_route];
        std::size_t position = 0;
        long double delta = std::numeric_limits<long double>::infinity();
        for(std::size_t candidate = 0;
            candidate <= route.size(); ++candidate){
            const long double candidate_delta =
                construction_detail::insertion_distance_delta(
                    distance, route, depot, customer, candidate
                );
            if(candidate_delta < delta){
                delta = candidate_delta;
                position = candidate;
            }
        }
        route.insert(
            route.begin()
                + static_cast<std::vector<int>::difference_type>(position),
            static_cast<int>(customer)
        );
        load[best_route] += demand[customer];
        assigned[customer] = 1;
        ++assigned_count;
    }
    return routes;
}

template<class DistanceMatrix>
std::vector<std::vector<int>> mole_jameson_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double vehicle_capacity,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot = 0
) {
    return construction_detail::time_window_insertion_routes(
        distance, demand, vehicle_capacity, time_window, depot,
        construction_detail::TimeInsertionRule::mole_jameson
    );
}

template<class DistanceMatrix>
std::vector<std::vector<int>> solomon_i1_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double vehicle_capacity,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot = 0
) {
    return construction_detail::time_window_insertion_routes(
        distance, demand, vehicle_capacity, time_window, depot,
        construction_detail::TimeInsertionRule::solomon_i1
    );
}

template<class DistanceMatrix>
std::vector<std::vector<int>> solomon_i2_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double vehicle_capacity,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot = 0
) {
    return construction_detail::time_window_insertion_routes(
        distance, demand, vehicle_capacity, time_window, depot,
        construction_detail::TimeInsertionRule::solomon_i2
    );
}

template<class DistanceMatrix>
std::vector<std::vector<int>> solomon_i3_routes(
    const DistanceMatrix& distance,
    const std::vector<long double>& demand,
    long double vehicle_capacity,
    const std::vector<RoutingTimeWindow>& time_window,
    std::size_t depot = 0
) {
    return construction_detail::time_window_insertion_routes(
        distance, demand, vehicle_capacity, time_window, depot,
        construction_detail::TimeInsertionRule::solomon_i3
    );
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_VEHICLE_ROUTING_CONSTRUCTION_HPP_INCLUDED
