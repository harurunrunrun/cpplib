// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/routing/route_construction.hpp"

namespace {

void require(bool condition, const char* message){
    if(!condition) throw std::runtime_error(message);
}

void validate_tour(const std::vector<int>& tour, std::size_t n){
    require(tour.size() == n, "tour size mismatch");
    std::vector<int> sorted = tour;
    std::sort(sorted.begin(), sorted.end());
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        require(
            sorted[vertex] == static_cast<int>(vertex),
            "tour is not a permutation"
        );
    }
}

void validate_customer_order(
    const std::vector<int>& order,
    std::size_t n,
    std::size_t depot
){
    require(order.size() == (n == 0 ? 0 : n - 1), "customer order size mismatch");
    std::vector<unsigned char> seen(n, 0);
    if(n != 0) seen[depot] = 1;
    for(const int vertex_value : order){
        require(vertex_value >= 0, "negative customer");
        const std::size_t vertex = static_cast<std::size_t>(vertex_value);
        require(vertex < n && seen[vertex] == 0, "invalid customer order");
        seen[vertex] = 1;
    }
    for(const unsigned char value : seen){
        require(value != 0, "customer is missing");
    }
}

void validate_routes(
    const std::vector<std::vector<int>>& routes,
    const std::vector<long long>& demand,
    long long capacity,
    std::size_t depot
){
    const std::size_t n = demand.size();
    std::vector<unsigned char> seen(n, 0);
    if(n != 0) seen[depot] = 1;
    for(const auto& route : routes){
        require(!route.empty(), "empty route");
        long long load = 0;
        for(const int vertex_value : route){
            require(vertex_value >= 0, "negative route vertex");
            const std::size_t vertex = static_cast<std::size_t>(vertex_value);
            require(
                vertex < n && vertex != depot && seen[vertex] == 0,
                "route customer is invalid or duplicated"
            );
            seen[vertex] = 1;
            load += demand[vertex];
        }
        require(load <= capacity, "route exceeds capacity");
    }
    for(const unsigned char value : seen){
        require(value != 0, "route customer is missing");
    }
}

}  // namespace

int main(){
    using namespace approximate::routing;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    {
        const std::vector<std::vector<long long>> empty_distance;
        const std::vector<long long> empty_demand;
        require(
            sequential_insertion_routes(
                empty_distance, empty_demand, 0LL
            ).empty(),
            "empty sequential insertion failed"
        );
        require(
            multi_fragment_tour(empty_distance).empty(),
            "empty multi-fragment failed"
        );
        require(
            angle_first_order({}).empty(),
            "empty angle-first failed"
        );
        require(
            nearest_depot_assignment(empty_distance, {}).empty(),
            "empty nearest-depot failed"
        );
    }

    {
        const std::vector<std::vector<long long>> distance{
            {0, 1, 4, 1},
            {1, 0, 1, 4},
            {4, 1, 0, 1},
            {1, 4, 1, 0}
        };
        require(
            multi_fragment_tour(distance) == std::vector<int>({2, 1, 0, 3}),
            "multi-fragment tie-break mismatch"
        );
        require(
            path_cheapest_arc_tour(distance)
                == std::vector<int>({0, 1, 2, 3}),
            "path cheapest-arc tie-break mismatch"
        );
        require(
            giant_tour(distance) == std::vector<int>({1, 2, 3}),
            "giant-tour tie-break mismatch"
        );
    }
    {
        const std::vector<RoutingPoint> points{
            {0, 0}, {1, 0}, {2, 0}, {0, 1}, {-1, 0}, {0, -1}
        };
        require(
            angle_first_order(points)
                == std::vector<int>({5, 1, 2, 3, 4}),
            "angle-first tie-break mismatch"
        );
    }
    {
        const std::vector<std::vector<long long>> distance{
            {0, 2, 4, 9},
            {2, 0, 7, 1},
            {4, 5, 0, 4},
            {9, 1, 4, 0}
        };
        require(
            nearest_depot_assignment(distance, {0, 3})
                == std::vector<int>({0, 3, 0, 3}),
            "nearest-depot tie-break mismatch"
        );
    }
    {
        const std::vector<long long> coordinate{0, 1, 2, 10, 11};
        std::vector<std::vector<long long>> distance(
            coordinate.size(),
            std::vector<long long>(coordinate.size())
        );
        for(std::size_t first = 0; first < coordinate.size(); ++first){
            for(std::size_t second = 0; second < coordinate.size(); ++second){
                distance[first][second] = std::abs(
                    coordinate[first] - coordinate[second]
                );
            }
        }
        const std::vector<long long> demand{0, 1, 1, 1, 1};
        require(
            split_tour(distance, {1, 2, 3, 4}, demand, 2LL)
                == std::vector<std::vector<int>>({{1, 2}, {3, 4}}),
            "split optimum reconstruction mismatch"
        );
    }

    int case_count = 0;
    std::cin >> case_count;
    while(case_count-- > 0){
        std::size_t n = 0;
        long long capacity = 0;
        std::cin >> n >> capacity;
        std::vector<RoutingPoint> points(n);
        for(auto& point : points){
            std::cin >> point.x >> point.y;
        }
        std::vector<long long> demand(n);
        for(long long& value : demand) std::cin >> value;
        std::vector<std::vector<long long>> distance(
            n, std::vector<long long>(n)
        );
        for(auto& row : distance){
            for(long long& value : row) std::cin >> value;
        }
        const std::size_t depot = 0;

        const auto sequential = sequential_insertion_routes(
            distance, demand, capacity, depot
        );
        validate_routes(sequential, demand, capacity, depot);

        const auto parallel = parallel_insertion_routes(
            distance,
            demand,
            capacity,
            n == 0 ? 0 : std::max<std::size_t>(1, n - 1),
            depot
        );
        validate_routes(parallel, demand, capacity, depot);

        validate_tour(multi_fragment_tour(distance), n);

        const auto clarke = clarke_wright_routes(
            distance, demand, capacity, depot
        );
        const auto parallel_savings = parallel_savings_routes(
            distance, demand, capacity, depot
        );
        require(clarke == parallel_savings, "Clarke-Wright alias mismatch");
        validate_routes(clarke, demand, capacity, depot);
        validate_routes(
            sequential_savings_routes(distance, demand, capacity, depot),
            demand,
            capacity,
            depot
        );

        const auto angular = angle_first_order(points, depot);
        validate_customer_order(angular, n, depot);
        validate_routes(
            sweep_routes(distance, points, demand, capacity, depot),
            demand,
            capacity,
            depot
        );
        validate_routes(
            petal_routes(distance, points, demand, capacity, depot),
            demand,
            capacity,
            depot
        );
        validate_routes(
            cluster_first_route_second(
                distance, points, demand, capacity, depot
            ),
            demand,
            capacity,
            depot
        );

        const auto giant = giant_tour(distance, depot);
        validate_customer_order(giant, n, depot);
        validate_routes(
            split_tour(distance, giant, demand, capacity, depot),
            demand,
            capacity,
            depot
        );
        validate_routes(
            route_first_cluster_second(
                distance, demand, capacity, depot
            ),
            demand,
            capacity,
            depot
        );

        const std::vector<int> depots = n < 2
            ? std::vector<int>{0}
            : std::vector<int>{0, static_cast<int>(n - 1)};
        const auto assignment = nearest_depot_assignment(distance, depots);
        require(assignment.size() == n, "nearest-depot size mismatch");
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            require(
                std::find(
                    depots.begin(), depots.end(), assignment[vertex]
                ) != depots.end(),
                "nearest-depot chose a non-depot"
            );
        }

        validate_tour(path_cheapest_arc_tour(distance), n);
        validate_routes(
            nearest_merger_routes(distance, demand, capacity, depot),
            demand,
            capacity,
            depot
        );

        if(n > 1){
            bool rejected = false;
            try{
                static_cast<void>(parallel_insertion_routes(
                    distance, demand, capacity, 0, depot
                ));
            }catch(const std::invalid_argument&){
                rejected = true;
            }
            require(rejected, "zero vehicle_count was accepted");

            std::vector<int> malformed(n - 1, static_cast<int>(depot));
            rejected = false;
            try{
                static_cast<void>(split_tour(
                    distance, malformed, demand, capacity, depot
                ));
            }catch(const std::invalid_argument&){
                rejected = true;
            }
            require(rejected, "malformed giant tour was accepted");
        }
        std::cout << "OK\n";
    }
}
