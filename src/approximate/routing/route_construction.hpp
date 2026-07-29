#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_ROUTE_CONSTRUCTION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_ROUTE_CONSTRUCTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

struct RoutingPoint {
    long double x = 0;
    long double y = 0;
};

namespace internal {

template<class DemandContainer>
using demand_value_t = std::remove_cv_t<std::remove_reference_t<decltype(
    std::declval<const DemandContainer&>()[std::size_t{}]
)>>;

inline void validate_routing_vertex_count(std::size_t n){
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("the vertex count does not fit in int");
    }
}

template<class DistanceMatrix, class DemandContainer>
std::size_t validate_capacitated_instance(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    std::size_t depot
){
    using Demand = demand_value_t<DemandContainer>;
    const std::size_t n = validate_distance_matrix(distance);
    validate_routing_vertex_count(n);
    if(demand.size() != n){
        throw std::invalid_argument(
            "demand size must equal the distance matrix size"
        );
    }
    if(capacity < Demand{}){
        throw std::invalid_argument("capacity must be nonnegative");
    }
    if(n == 0){
        if(depot != 0) throw std::out_of_range("depot is outside an empty matrix");
        return 0;
    }
    if(depot >= n) throw std::out_of_range("depot is outside the matrix");
    if(demand[depot] < Demand{} || Demand{} < demand[depot]){
        throw std::invalid_argument("the depot demand must be zero");
    }
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(demand[vertex] < Demand{}){
            throw std::invalid_argument("customer demand must be nonnegative");
        }
        if(vertex != depot && capacity < demand[vertex]){
            throw std::invalid_argument(
                "a customer demand exceeds the vehicle capacity"
            );
        }
    }
    return n;
}

template<class Demand>
bool capacity_allows(Demand load, Demand added, Demand capacity){
    return !(capacity < added) && !(capacity - added < load);
}

template<class DistanceMatrix>
auto closed_route_cost(
    const DistanceMatrix& distance,
    const std::vector<int>& route,
    std::size_t depot
) -> distance_cost_t<DistanceMatrix> {
    using Cost = distance_cost_t<DistanceMatrix>;
    if(route.empty()) return Cost{};
    Cost result = distance[depot][static_cast<std::size_t>(route.front())];
    for(std::size_t index = 1; index < route.size(); ++index){
        result += distance[
            static_cast<std::size_t>(route[index - 1])
        ][static_cast<std::size_t>(route[index])];
    }
    result += distance[static_cast<std::size_t>(route.back())][depot];
    return result;
}

template<class DistanceMatrix>
auto route_insertion_delta(
    const DistanceMatrix& distance,
    const std::vector<int>& route,
    std::size_t depot,
    std::size_t vertex,
    std::size_t position
) -> distance_cost_t<DistanceMatrix> {
    if(route.empty()){
        return distance[depot][vertex] + distance[vertex][depot];
    }
    const std::size_t previous = position == 0
        ? depot
        : static_cast<std::size_t>(route[position - 1]);
    const std::size_t next = position == route.size()
        ? depot
        : static_cast<std::size_t>(route[position]);
    return distance[previous][vertex] + distance[vertex][next]
        - distance[previous][next];
}

template<class DistanceMatrix>
std::pair<std::size_t, distance_cost_t<DistanceMatrix>>
best_route_insertion(
    const DistanceMatrix& distance,
    const std::vector<int>& route,
    std::size_t depot,
    std::size_t vertex
){
    std::size_t best_position = 0;
    auto best_delta = route_insertion_delta(
        distance, route, depot, vertex, 0
    );
    for(std::size_t position = 1; position <= route.size(); ++position){
        const auto delta = route_insertion_delta(
            distance, route, depot, vertex, position
        );
        if(delta < best_delta){
            best_delta = delta;
            best_position = position;
        }
    }
    return {best_position, best_delta};
}

template<class DistanceMatrix>
std::size_t validate_depot(
    const DistanceMatrix& distance,
    std::size_t depot
){
    const std::size_t n = validate_distance_matrix(distance);
    validate_routing_vertex_count(n);
    if(n == 0){
        if(depot != 0) throw std::out_of_range("depot is outside an empty matrix");
    }else if(depot >= n){
        throw std::out_of_range("depot is outside the matrix");
    }
    return n;
}

class RouteConstructionDsu {
private:
    std::vector<int> parent_;
    std::vector<int> size_;

public:
    explicit RouteConstructionDsu(std::size_t n)
        : parent_(n), size_(n, 1)
    {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    int leader(int vertex){
        int root = vertex;
        while(parent_[static_cast<std::size_t>(root)] != root){
            root = parent_[static_cast<std::size_t>(root)];
        }
        while(parent_[static_cast<std::size_t>(vertex)] != vertex){
            const int next = parent_[static_cast<std::size_t>(vertex)];
            parent_[static_cast<std::size_t>(vertex)] = root;
            vertex = next;
        }
        return root;
    }

    bool same(int first, int second){
        return leader(first) == leader(second);
    }

    void merge(int first, int second){
        first = leader(first);
        second = leader(second);
        if(first == second) return;
        if(size_[static_cast<std::size_t>(first)]
            < size_[static_cast<std::size_t>(second)]
        ) std::swap(first, second);
        parent_[static_cast<std::size_t>(second)] = first;
        size_[static_cast<std::size_t>(first)]
            += size_[static_cast<std::size_t>(second)];
    }
};

template<class Cost>
struct RouteConstructionEdge {
    Cost cost;
    int from;
    int to;
};

template<class Cost>
bool edge_cost_less(
    const RouteConstructionEdge<Cost>& first,
    const RouteConstructionEdge<Cost>& second
){
    if(first.cost < second.cost) return true;
    if(second.cost < first.cost) return false;
    if(first.from != second.from) return first.from < second.from;
    return first.to < second.to;
}

inline std::vector<int> path_from_adjacency(
    const std::vector<std::vector<int>>& graph
){
    if(graph.empty()) return {};
    if(graph.size() == 1) return {0};
    std::size_t start = graph.size();
    for(std::size_t vertex = 0; vertex < graph.size(); ++vertex){
        if(graph[vertex].size() == 1){
            start = vertex;
            break;
        }
    }
    if(start == graph.size()){
        throw std::logic_error("a Hamiltonian path endpoint was not found");
    }
    std::vector<int> result;
    result.reserve(graph.size());
    int previous = -1;
    int current = static_cast<int>(start);
    while(current >= 0){
        result.push_back(current);
        int next = -1;
        for(const int candidate : graph[static_cast<std::size_t>(current)]){
            if(candidate != previous){
                next = candidate;
                break;
            }
        }
        previous = current;
        current = next;
    }
    if(result.size() != graph.size()){
        throw std::logic_error("Hamiltonian path reconstruction failed");
    }
    return result;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> pack_in_order(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    std::size_t depot,
    const std::vector<int>& order
){
    static_cast<void>(distance);
    using Demand = demand_value_t<DemandContainer>;
    std::vector<std::vector<int>> routes;
    Demand load{};
    for(const int vertex_value : order){
        const std::size_t vertex = static_cast<std::size_t>(vertex_value);
        if(
            routes.empty()
            || !capacity_allows(load, demand[vertex], capacity)
        ){
            routes.push_back({});
            load = Demand{};
        }
        routes.back().push_back(vertex_value);
        load += demand[vertex];
    }
    static_cast<void>(depot);
    return routes;
}

template<class DistanceMatrix>
auto routes_cost(
    const DistanceMatrix& distance,
    const std::vector<std::vector<int>>& routes,
    std::size_t depot
) -> distance_cost_t<DistanceMatrix> {
    using Cost = distance_cost_t<DistanceMatrix>;
    Cost result{};
    for(const auto& route : routes){
        result += closed_route_cost(distance, route, depot);
    }
    return result;
}

template<class DistanceMatrix>
void cheapest_reorder(
    const DistanceMatrix& distance,
    std::vector<int>& route,
    std::size_t depot
){
    if(route.size() < 2) return;
    const std::vector<int> order = route;
    route.assign(1, order.front());
    for(std::size_t index = 1; index < order.size(); ++index){
        const std::size_t vertex = static_cast<std::size_t>(order[index]);
        const auto choice = best_route_insertion(
            distance, route, depot, vertex
        );
        route.insert(
            route.begin()
                + static_cast<std::vector<int>::difference_type>(choice.first),
            order[index]
        );
    }
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> parallel_savings_impl(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    std::size_t depot
){
    using Demand = demand_value_t<DemandContainer>;
    using Cost = distance_cost_t<DistanceMatrix>;
    const std::size_t n = validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    if(n < 2) return {};

    std::vector<std::vector<int>> routes;
    std::vector<Demand> loads;
    std::vector<std::size_t> owner(n, n);
    routes.reserve(n - 1);
    loads.reserve(n - 1);
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex == depot) continue;
        owner[vertex] = routes.size();
        routes.push_back({static_cast<int>(vertex)});
        loads.push_back(demand[vertex]);
    }

    std::vector<RouteConstructionEdge<Cost>> savings;
    savings.reserve((n - 1) * (n - 2));
    for(std::size_t from = 0; from < n; ++from){
        if(from == depot) continue;
        for(std::size_t to = 0; to < n; ++to){
            if(to == depot || from == to) continue;
            savings.push_back({
                distance[from][depot] + distance[depot][to]
                    - distance[from][to],
                static_cast<int>(from),
                static_cast<int>(to)
            });
        }
    }
    std::sort(
        savings.begin(),
        savings.end(),
        [](const auto& first, const auto& second){
            if(first.cost < second.cost) return false;
            if(second.cost < first.cost) return true;
            if(first.from != second.from) return first.from < second.from;
            return first.to < second.to;
        }
    );

    for(const auto& saving : savings){
        if(saving.cost < Cost{}) break;
        const std::size_t from = static_cast<std::size_t>(saving.from);
        const std::size_t to = static_cast<std::size_t>(saving.to);
        std::size_t first_id = owner[from];
        std::size_t second_id = owner[to];
        if(first_id == second_id) continue;
        auto& first = routes[first_id];
        auto& second = routes[second_id];
        if(first.empty() || second.empty()) continue;
        if(
            first.front() != saving.from
            && first.back() != saving.from
        ) continue;
        if(
            second.front() != saving.to
            && second.back() != saving.to
        ) continue;
        if(!capacity_allows(loads[first_id], loads[second_id], capacity)){
            continue;
        }
        if(first.front() == saving.from) std::reverse(first.begin(), first.end());
        if(second.back() == saving.to) std::reverse(second.begin(), second.end());
        first.insert(first.end(), second.begin(), second.end());
        loads[first_id] += loads[second_id];
        for(const int vertex : second){
            owner[static_cast<std::size_t>(vertex)] = first_id;
        }
        second.clear();
        loads[second_id] = Demand{};
    }

    std::vector<std::vector<int>> result;
    for(auto& route : routes){
        if(!route.empty()) result.push_back(std::move(route));
    }
    return result;
}

}  // namespace internal

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> sequential_insertion_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    using Demand = internal::demand_value_t<DemandContainer>;
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    std::vector<unsigned char> used(n, 0);
    if(n != 0) used[depot] = 1;
    std::size_t remaining = n == 0 ? 0 : n - 1;
    std::vector<std::vector<int>> routes;
    while(remaining != 0){
        std::vector<int> route;
        Demand load{};
        while(true){
            std::size_t selected = n;
            std::size_t selected_position = 0;
            internal::distance_cost_t<DistanceMatrix> selected_delta{};
            for(std::size_t vertex = 0; vertex < n; ++vertex){
                if(
                    used[vertex] != 0
                    || !internal::capacity_allows(
                        load, demand[vertex], capacity
                    )
                ) continue;
                const auto [position, delta] = internal::best_route_insertion(
                    distance, route, depot, vertex
                );
                if(selected == n || delta < selected_delta){
                    selected = vertex;
                    selected_position = position;
                    selected_delta = delta;
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
            used[selected] = 1;
            --remaining;
        }
        if(route.empty()){
            throw std::logic_error("sequential insertion made no progress");
        }
        routes.push_back(std::move(route));
    }
    return routes;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> parallel_insertion_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t vehicle_count,
    std::size_t depot = 0
){
    using Demand = internal::demand_value_t<DemandContainer>;
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    const std::size_t customers = n == 0 ? 0 : n - 1;
    if(customers != 0 && vehicle_count == 0){
        throw std::invalid_argument("vehicle_count must be positive");
    }
    std::vector<unsigned char> used(n, 0);
    if(n != 0) used[depot] = 1;
    std::vector<std::vector<int>> routes;
    std::vector<Demand> loads;
    const std::size_t seed_count = std::min(vehicle_count, customers);
    for(std::size_t seed_index = 0; seed_index < seed_count; ++seed_index){
        std::size_t selected = n;
        Cost selected_score{};
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            const Cost score = distance[depot][vertex] + distance[vertex][depot];
            if(selected == n || selected_score < score){
                selected = vertex;
                selected_score = score;
            }
        }
        routes.push_back({static_cast<int>(selected)});
        loads.push_back(demand[selected]);
        used[selected] = 1;
    }

    std::size_t remaining = customers - seed_count;
    while(remaining != 0){
        std::size_t selected = n;
        std::size_t selected_route = 0;
        std::size_t selected_position = 0;
        Cost selected_delta{};
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            for(std::size_t route_id = 0; route_id < routes.size(); ++route_id){
                if(!internal::capacity_allows(
                    loads[route_id], demand[vertex], capacity
                )) continue;
                const auto [position, delta] = internal::best_route_insertion(
                    distance, routes[route_id], depot, vertex
                );
                if(selected == n || delta < selected_delta){
                    selected = vertex;
                    selected_route = route_id;
                    selected_position = position;
                    selected_delta = delta;
                }
            }
        }
        if(selected == n){
            throw std::invalid_argument(
                "the greedy insertion cannot fit all customers in vehicle_count routes"
            );
        }
        auto& route = routes[selected_route];
        route.insert(
            route.begin()
                + static_cast<std::vector<int>::difference_type>(
                    selected_position
                ),
            static_cast<int>(selected)
        );
        loads[selected_route] += demand[selected];
        used[selected] = 1;
        --remaining;
    }
    return routes;
}

template<class DistanceMatrix>
std::vector<int> multi_fragment_tour(const DistanceMatrix& distance){
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_routing_vertex_count(n);
    if(n < 2) return n == 0 ? std::vector<int>{} : std::vector<int>{0};

    std::vector<internal::RouteConstructionEdge<Cost>> edges;
    edges.reserve(n * (n - 1) / 2);
    for(std::size_t first = 0; first < n; ++first){
        for(std::size_t second = first + 1; second < n; ++second){
            edges.push_back({
                distance[first][second],
                static_cast<int>(first),
                static_cast<int>(second)
            });
        }
    }
    std::sort(
        edges.begin(), edges.end(),
        internal::edge_cost_less<Cost>
    );
    internal::RouteConstructionDsu dsu(n);
    std::vector<int> degree(n, 0);
    std::vector<std::vector<int>> graph(n);
    std::size_t selected_count = 0;
    for(const auto& edge : edges){
        const std::size_t from = static_cast<std::size_t>(edge.from);
        const std::size_t to = static_cast<std::size_t>(edge.to);
        if(degree[from] == 2 || degree[to] == 2 || dsu.same(edge.from, edge.to)){
            continue;
        }
        ++degree[from];
        ++degree[to];
        graph[from].push_back(edge.to);
        graph[to].push_back(edge.from);
        dsu.merge(edge.from, edge.to);
        if(++selected_count == n - 1) break;
    }
    if(selected_count != n - 1){
        throw std::logic_error("multi-fragment construction failed");
    }
    return internal::path_from_adjacency(graph);
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> parallel_savings_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    return internal::parallel_savings_impl(
        distance, demand, capacity, depot
    );
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> clarke_wright_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    return parallel_savings_routes(distance, demand, capacity, depot);
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> sequential_savings_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    using Demand = internal::demand_value_t<DemandContainer>;
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    std::vector<unsigned char> used(n, 0);
    if(n != 0) used[depot] = 1;
    std::vector<std::vector<int>> routes;
    for(std::size_t seed = 0; seed < n; ++seed){
        if(used[seed] != 0) continue;
        std::vector<int> route{static_cast<int>(seed)};
        Demand load = demand[seed];
        used[seed] = 1;
        while(true){
            std::size_t selected = n;
            bool insert_front = false;
            Cost selected_saving{};
            for(std::size_t vertex = 0; vertex < n; ++vertex){
                if(
                    used[vertex] != 0
                    || !internal::capacity_allows(
                        load, demand[vertex], capacity
                    )
                ) continue;
                const std::size_t front = static_cast<std::size_t>(route.front());
                const std::size_t back = static_cast<std::size_t>(route.back());
                const Cost front_saving =
                    distance[depot][front] + distance[vertex][depot]
                    - distance[vertex][front];
                const Cost back_saving =
                    distance[back][depot] + distance[depot][vertex]
                    - distance[back][vertex];
                const bool front_is_better = back_saving < front_saving;
                const Cost saving = front_is_better
                    ? front_saving
                    : back_saving;
                if(
                    selected == n
                    || selected_saving < saving
                ){
                    selected = vertex;
                    insert_front = front_is_better;
                    selected_saving = saving;
                }
            }
            if(selected == n || selected_saving < Cost{}) break;
            if(insert_front){
                route.insert(route.begin(), static_cast<int>(selected));
            }else{
                route.push_back(static_cast<int>(selected));
            }
            load += demand[selected];
            used[selected] = 1;
        }
        routes.push_back(std::move(route));
    }
    return routes;
}

inline std::vector<int> angle_first_order(
    const std::vector<RoutingPoint>& points,
    std::size_t depot = 0
){
    const std::size_t n = points.size();
    internal::validate_routing_vertex_count(n);
    if(n == 0){
        if(depot != 0) throw std::out_of_range("depot is outside an empty point set");
        return {};
    }
    if(depot >= n) throw std::out_of_range("depot is outside the point set");
    struct PolarVertex {
        long double angle;
        long double squared_distance;
        int vertex;
    };
    std::vector<PolarVertex> polar;
    polar.reserve(n - 1);
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex == depot) continue;
        const long double dx = points[vertex].x - points[depot].x;
        const long double dy = points[vertex].y - points[depot].y;
        polar.push_back({
            std::atan2(dy, dx),
            dx * dx + dy * dy,
            static_cast<int>(vertex)
        });
    }
    std::sort(
        polar.begin(), polar.end(),
        [](const PolarVertex& first, const PolarVertex& second){
            if(first.angle != second.angle) return first.angle < second.angle;
            if(first.squared_distance != second.squared_distance){
                return first.squared_distance < second.squared_distance;
            }
            return first.vertex < second.vertex;
        }
    );
    std::vector<int> result;
    result.reserve(polar.size());
    for(const auto& item : polar) result.push_back(item.vertex);
    return result;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> sweep_routes(
    const DistanceMatrix& distance,
    const std::vector<RoutingPoint>& points,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    if(points.size() != n){
        throw std::invalid_argument(
            "point count must equal the distance matrix size"
        );
    }
    return internal::pack_in_order(
        distance, demand, capacity, depot,
        angle_first_order(points, depot)
    );
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> petal_routes(
    const DistanceMatrix& distance,
    const std::vector<RoutingPoint>& points,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    if(points.size() != n){
        throw std::invalid_argument(
            "point count must equal the distance matrix size"
        );
    }
    const std::vector<int> angular = angle_first_order(points, depot);
    if(angular.empty()) return {};
    std::vector<std::vector<int>> best;
    internal::distance_cost_t<DistanceMatrix> best_cost{};
    bool initialized = false;
    std::vector<int> order = angular;
    for(std::size_t shift = 0; shift < angular.size(); ++shift){
        if(shift != 0){
            std::rotate(order.begin(), order.begin() + 1, order.end());
        }
        auto candidate = internal::pack_in_order(
            distance, demand, capacity, depot, order
        );
        const auto candidate_cost = internal::routes_cost(
            distance, candidate, depot
        );
        if(!initialized || candidate_cost < best_cost){
            initialized = true;
            best_cost = candidate_cost;
            best = std::move(candidate);
        }
    }
    return best;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> cluster_first_route_second(
    const DistanceMatrix& distance,
    const std::vector<RoutingPoint>& points,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    auto routes = sweep_routes(
        distance, points, demand, capacity, depot
    );
    for(auto& route : routes){
        internal::cheapest_reorder(distance, route, depot);
    }
    return routes;
}

template<class DistanceMatrix>
std::vector<int> giant_tour(
    const DistanceMatrix& distance,
    std::size_t depot = 0
){
    const std::size_t n = internal::validate_depot(distance, depot);
    if(n < 2) return {};
    std::vector<unsigned char> used(n, 0);
    used[depot] = 1;
    std::vector<int> result;
    result.reserve(n - 1);
    std::size_t current = depot;
    while(result.size() != n - 1){
        std::size_t selected = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            if(
                selected == n
                || distance[current][vertex] < distance[current][selected]
            ) selected = vertex;
        }
        result.push_back(static_cast<int>(selected));
        used[selected] = 1;
        current = selected;
    }
    return result;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> split_tour(
    const DistanceMatrix& distance,
    const std::vector<int>& giant_order,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    using Demand = internal::demand_value_t<DemandContainer>;
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    const std::size_t customer_count = n == 0 ? 0 : n - 1;
    if(giant_order.size() != customer_count){
        throw std::invalid_argument(
            "giant_order must contain every non-depot vertex exactly once"
        );
    }
    std::vector<unsigned char> present(n, 0);
    if(n != 0) present[depot] = 1;
    for(const int vertex_value : giant_order){
        internal::validate_vertex(vertex_value, n);
        const std::size_t vertex = static_cast<std::size_t>(vertex_value);
        if(vertex == depot || present[vertex] != 0){
            throw std::invalid_argument(
                "giant_order must contain every non-depot vertex exactly once"
            );
        }
        present[vertex] = 1;
    }
    if(customer_count == 0) return {};

    std::vector<Cost> best(customer_count + 1);
    std::vector<unsigned char> reachable(customer_count + 1, 0);
    std::vector<std::size_t> previous(customer_count + 1, customer_count + 1);
    reachable[0] = 1;
    for(std::size_t first = 0; first < customer_count; ++first){
        if(reachable[first] == 0) continue;
        Demand load{};
        Cost path{};
        for(std::size_t last = first; last < customer_count; ++last){
            const std::size_t vertex = static_cast<std::size_t>(
                giant_order[last]
            );
            if(!internal::capacity_allows(load, demand[vertex], capacity)){
                break;
            }
            load += demand[vertex];
            if(last == first){
                path = distance[depot][vertex];
            }else{
                path += distance[
                    static_cast<std::size_t>(giant_order[last - 1])
                ][vertex];
            }
            const Cost candidate = best[first] + path + distance[vertex][depot];
            const std::size_t next = last + 1;
            if(reachable[next] == 0 || candidate < best[next]){
                reachable[next] = 1;
                best[next] = candidate;
                previous[next] = first;
            }
        }
    }
    if(reachable[customer_count] == 0){
        throw std::invalid_argument("giant_order cannot be split feasibly");
    }
    std::vector<std::vector<int>> reversed_routes;
    for(std::size_t end = customer_count; end != 0; end = previous[end]){
        const std::size_t begin = previous[end];
        reversed_routes.emplace_back(
            giant_order.begin()
                + static_cast<std::vector<int>::difference_type>(begin),
            giant_order.begin()
                + static_cast<std::vector<int>::difference_type>(end)
        );
    }
    std::reverse(reversed_routes.begin(), reversed_routes.end());
    return reversed_routes;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> route_first_cluster_second(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    return split_tour(
        distance, giant_tour(distance, depot), demand, capacity, depot
    );
}

template<class DistanceMatrix>
std::vector<int> nearest_depot_assignment(
    const DistanceMatrix& distance,
    const std::vector<int>& depots
){
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_routing_vertex_count(n);
    if(n == 0){
        if(!depots.empty()){
            throw std::out_of_range("a depot is outside an empty matrix");
        }
        return {};
    }
    if(depots.empty()){
        throw std::invalid_argument("at least one depot is required");
    }
    std::vector<unsigned char> is_depot(n, 0);
    for(const int depot_value : depots){
        internal::validate_vertex(depot_value, n);
        const std::size_t depot = static_cast<std::size_t>(depot_value);
        if(is_depot[depot] != 0){
            throw std::invalid_argument("depots must be distinct");
        }
        is_depot[depot] = 1;
    }
    std::vector<int> assignment(n);
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(is_depot[vertex] != 0){
            assignment[vertex] = static_cast<int>(vertex);
            continue;
        }
        int selected = depots.front();
        for(std::size_t index = 1; index < depots.size(); ++index){
            const int candidate = depots[index];
            if(
                distance[vertex][static_cast<std::size_t>(candidate)]
                < distance[vertex][static_cast<std::size_t>(selected)]
            ) selected = candidate;
        }
        assignment[vertex] = selected;
    }
    return assignment;
}

template<class DistanceMatrix>
std::vector<int> path_cheapest_arc_tour(const DistanceMatrix& distance){
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    internal::validate_routing_vertex_count(n);
    if(n < 2) return n == 0 ? std::vector<int>{} : std::vector<int>{0};
    std::vector<internal::RouteConstructionEdge<Cost>> arcs;
    arcs.reserve(n * (n - 1));
    for(std::size_t from = 0; from < n; ++from){
        for(std::size_t to = 0; to < n; ++to){
            if(from == to) continue;
            arcs.push_back({
                distance[from][to],
                static_cast<int>(from),
                static_cast<int>(to)
            });
        }
    }
    std::sort(
        arcs.begin(), arcs.end(),
        internal::edge_cost_less<Cost>
    );
    internal::RouteConstructionDsu dsu(n);
    std::vector<int> next(n, -1);
    std::vector<int> previous(n, -1);
    std::size_t selected_count = 0;
    for(const auto& arc : arcs){
        const std::size_t from = static_cast<std::size_t>(arc.from);
        const std::size_t to = static_cast<std::size_t>(arc.to);
        if(
            next[from] >= 0
            || previous[to] >= 0
            || dsu.same(arc.from, arc.to)
        ) continue;
        next[from] = arc.to;
        previous[to] = arc.from;
        dsu.merge(arc.from, arc.to);
        if(++selected_count == n - 1) break;
    }
    if(selected_count != n - 1){
        throw std::logic_error("path cheapest-arc construction failed");
    }
    std::size_t start = 0;
    while(previous[start] >= 0) ++start;
    std::vector<int> result;
    result.reserve(n);
    for(int current = static_cast<int>(start); current >= 0; ){
        result.push_back(current);
        current = next[static_cast<std::size_t>(current)];
    }
    if(result.size() != n){
        throw std::logic_error("path cheapest-arc reconstruction failed");
    }
    return result;
}

template<class DistanceMatrix, class DemandContainer>
std::vector<std::vector<int>> nearest_merger_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    internal::demand_value_t<DemandContainer> capacity,
    std::size_t depot = 0
){
    using Demand = internal::demand_value_t<DemandContainer>;
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_capacitated_instance(
        distance, demand, capacity, depot
    );
    std::vector<std::vector<int>> routes;
    std::vector<Demand> loads;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex == depot) continue;
        routes.push_back({static_cast<int>(vertex)});
        loads.push_back(demand[vertex]);
    }
    while(true){
        std::size_t selected_first = routes.size();
        std::size_t selected_second = routes.size();
        bool reverse_first = false;
        bool reverse_second = false;
        Cost selected_delta{};
        for(std::size_t first = 0; first < routes.size(); ++first){
            if(routes[first].empty()) continue;
            for(std::size_t second = first + 1; second < routes.size(); ++second){
                if(
                    routes[second].empty()
                    || !internal::capacity_allows(
                        loads[first], loads[second], capacity
                    )
                ) continue;
                for(int first_reverse = 0; first_reverse < 2; ++first_reverse){
                    const int from = first_reverse == 0
                        ? routes[first].back()
                        : routes[first].front();
                    for(int second_reverse = 0; second_reverse < 2; ++second_reverse){
                        const int to = second_reverse == 0
                            ? routes[second].front()
                            : routes[second].back();
                        const Cost delta = distance[
                            static_cast<std::size_t>(from)
                        ][static_cast<std::size_t>(to)]
                            - distance[static_cast<std::size_t>(from)][depot]
                            - distance[depot][static_cast<std::size_t>(to)];
                        if(
                            selected_first == routes.size()
                            || delta < selected_delta
                        ){
                            selected_first = first;
                            selected_second = second;
                            reverse_first = first_reverse != 0;
                            reverse_second = second_reverse != 0;
                            selected_delta = delta;
                        }
                    }
                }
            }
        }
        if(selected_first == routes.size()) break;
        auto& first = routes[selected_first];
        auto& second = routes[selected_second];
        if(reverse_first) std::reverse(first.begin(), first.end());
        if(reverse_second) std::reverse(second.begin(), second.end());
        first.insert(first.end(), second.begin(), second.end());
        second.clear();
        loads[selected_first] += loads[selected_second];
        loads[selected_second] = Demand{};
    }
    std::vector<std::vector<int>> result;
    for(auto& route : routes){
        if(!route.empty()) result.push_back(std::move(route));
    }
    return result;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_ROUTE_CONSTRUCTION_HPP_INCLUDED
