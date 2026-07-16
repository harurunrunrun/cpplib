#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

struct ResourceConstrainedEdge {
    int to;
    int resource_usage;
    long long cost;
};

struct ResourceConstrainedPathResult {
    long long cost;
    int resource_used;
};

inline std::optional<ResourceConstrainedPathResult>
minimum_cost_path_with_resource_limit(
    const std::vector<std::vector<ResourceConstrainedEdge>>& graph,
    int source,
    int target,
    int resource_limit
){
    if(graph.empty() ||
       graph.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       source < 0 ||
       target < 0 ||
       source >= static_cast<int>(graph.size()) ||
       target >= static_cast<int>(graph.size()) ||
       resource_limit < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid graph, endpoint, or resource limit "
            "(minimum_cost_path_with_resource_limit)."
        );
    }

    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 ||
               edge.to >= static_cast<int>(graph.size()) ||
               edge.resource_usage <= 0 ||
               edge.cost < 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid resource-constrained edge "
                    "(minimum_cost_path_with_resource_limit)."
                );
            }
        }
    }

    using Wide = __int128_t;
    const Wide infinity = Wide(1) << 126;
    std::vector<std::vector<Wide>> minimum_cost(
        static_cast<std::size_t>(resource_limit) + 1,
        std::vector<Wide>(graph.size(), infinity)
    );
    minimum_cost[0][static_cast<std::size_t>(source)] = 0;

    for(std::size_t used = 0; used < minimum_cost.size(); used++){
        const auto& current = minimum_cost[used];
        for(std::size_t vertex = 0; vertex < graph.size(); vertex++){
            if(current[vertex] == infinity) continue;
            for(const auto& edge: graph[vertex]){
                if(edge.resource_usage > resource_limit - static_cast<int>(used)) continue;
                const std::size_t next_used =
                    used + static_cast<std::size_t>(edge.resource_usage);
                Wide& destination =
                    minimum_cost[next_used]
                                [static_cast<std::size_t>(edge.to)];
                destination = std::min(
                    destination,
                    current[vertex] + static_cast<Wide>(edge.cost)
                );
            }
        }
    }

    Wide best_cost = infinity;
    int best_resource = -1;
    for(std::size_t used = 0; used < minimum_cost.size(); used++){
        const Wide candidate =
            minimum_cost[used]
                        [static_cast<std::size_t>(target)];
        if(candidate < best_cost){
            best_cost = candidate;
            best_resource = static_cast<int>(used);
        }
    }
    if(best_resource < 0) return std::nullopt;
    if(best_cost > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: result overflow "
            "(minimum_cost_path_with_resource_limit)."
        );
    }
    return ResourceConstrainedPathResult{
        static_cast<long long>(best_cost),
        best_resource
    };
}
