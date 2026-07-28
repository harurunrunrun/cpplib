#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <vector>

#include "dinic.hpp"

struct MinimumCostCirculationEdge{
    int from;
    int to;
    long long lower;
    long long upper;
    long long cost;
};

struct MinimumCostCirculationResult{
    __int128 cost;
    std::vector<long long> flow;
};

inline std::optional<MinimumCostCirculationResult>
minimum_cost_circulation(
    int vertex_count,
    const std::vector<MinimumCostCirculationEdge>& edges
){
    using Wide = __int128;
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(minimum_cost_circulation)."
        );
    }
    if(vertex_count > std::numeric_limits<int>::max() - 2)[[unlikely]]{
        throw std::overflow_error(
            "graph is too large (minimum_cost_circulation)"
        );
    }
    const std::size_t maximum_edge_pairs = static_cast<std::size_t>(
        std::numeric_limits<int>::max() / 2
    );
    if(edges.size() > maximum_edge_pairs
        || static_cast<std::size_t>(vertex_count)
            > maximum_edge_pairs - edges.size())[[unlikely]]{
        throw std::length_error(
            "too many residual edges (minimum_cost_circulation)"
        );
    }
    for(const auto& edge: edges){
        if(edge.from < 0 || edge.from >= vertex_count
            || edge.to < 0 || edge.to >= vertex_count
            || edge.lower < 0 || edge.upper < edge.lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid edge "
                "(minimum_cost_circulation)."
            );
        }
    }

    const int super_source = vertex_count;
    const int super_sink = vertex_count + 1;
    Dinic<long long> feasibility(vertex_count + 2);
    std::vector<Wide> balance(static_cast<std::size_t>(vertex_count), 0);
    std::vector<int> feasibility_edge;
    feasibility_edge.reserve(edges.size());
    for(const auto& edge: edges){
        feasibility_edge.push_back(feasibility.add_edge(
            edge.from, edge.to, edge.upper - edge.lower
        ));
        balance[static_cast<std::size_t>(edge.from)] -= edge.lower;
        balance[static_cast<std::size_t>(edge.to)] += edge.lower;
    }
    Wide required_wide = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const Wide delta = -balance[static_cast<std::size_t>(vertex)];
        if(delta > 0){
            if(delta > std::numeric_limits<long long>::max())[[unlikely]]{
                throw std::overflow_error(
                    "demand overflows long long "
                    "(minimum_cost_circulation)"
                );
            }
            feasibility.add_edge(
                vertex, super_sink, static_cast<long long>(delta)
            );
            required_wide += delta;
        }else if(delta < 0){
            if(-delta > std::numeric_limits<long long>::max())[[unlikely]]{
                throw std::overflow_error(
                    "supply overflows long long "
                    "(minimum_cost_circulation)"
                );
            }
            feasibility.add_edge(
                super_source, vertex, static_cast<long long>(-delta)
            );
        }
    }
    if(required_wide > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "total demand overflows long long "
            "(minimum_cost_circulation)"
        );
    }
    const long long required = static_cast<long long>(required_wide);
    if(feasibility.max_flow(
        super_source, super_sink, required
    ) != required){
        return std::nullopt;
    }

    MinimumCostCirculationResult result;
    result.flow.resize(edges.size());
    for(std::size_t index = 0; index < edges.size(); ++index){
        result.flow[index] = edges[index].lower
            + feasibility.edges[static_cast<std::size_t>(
                feasibility_edge[index]
            )].flow;
    }

    struct ResidualEdge{
        int from;
        int to;
        long long capacity;
        Wide scaled_cost;
        int original_edge;
        int direction;
    };
    const Wide scale = static_cast<Wide>(vertex_count) + 1;
    std::vector<ResidualEdge> residual_edges;
    residual_edges.reserve(edges.size() * 2);
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(std::size_t index = 0; index < edges.size(); ++index){
        const auto& edge = edges[index];
        const int id = static_cast<int>(residual_edges.size());
        graph[static_cast<std::size_t>(edge.from)].push_back(id);
        residual_edges.push_back({
            edge.from,
            edge.to,
            edge.upper - result.flow[index],
            static_cast<Wide>(edge.cost) * scale,
            static_cast<int>(index),
            1
        });
        graph[static_cast<std::size_t>(edge.to)].push_back(id ^ 1);
        residual_edges.push_back({
            edge.to,
            edge.from,
            result.flow[index] - edge.lower,
            -static_cast<Wide>(edge.cost) * scale,
            static_cast<int>(index),
            -1
        });
    }
    const auto push = [&](int edge_id, long long amount){
        auto& edge = residual_edges[static_cast<std::size_t>(edge_id)];
        auto& reverse =
            residual_edges[static_cast<std::size_t>(edge_id ^ 1)];
        edge.capacity -= amount;
        reverse.capacity += amount;
        result.flow[static_cast<std::size_t>(edge.original_edge)]
            += static_cast<long long>(edge.direction) * amount;
    };

    Wide maximum_scaled_cost = 0;
    for(const auto& edge: residual_edges){
        maximum_scaled_cost = std::max(
            maximum_scaled_cost,
            edge.scaled_cost < 0
                ? -edge.scaled_cost : edge.scaled_cost
        );
    }
    Wide epsilon = 1;
    while(epsilon < maximum_scaled_cost) epsilon <<= 1;
    std::vector<Wide> potential(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<Wide> excess(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<std::size_t> pointer(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<unsigned char> queued(
        static_cast<std::size_t>(vertex_count), 0
    );
    const auto reduced_cost = [&](const ResidualEdge& edge){
        return edge.scaled_cost
            + potential[static_cast<std::size_t>(edge.from)]
            - potential[static_cast<std::size_t>(edge.to)];
    };

    while(true){
        std::fill(excess.begin(), excess.end(), Wide(0));
        for(int edge_id = 0;
            edge_id < static_cast<int>(residual_edges.size());
            ++edge_id){
            auto& edge =
                residual_edges[static_cast<std::size_t>(edge_id)];
            if(edge.capacity <= 0 || reduced_cost(edge) >= 0) continue;
            const long long amount = edge.capacity;
            excess[static_cast<std::size_t>(edge.from)] -= amount;
            excess[static_cast<std::size_t>(edge.to)] += amount;
            push(edge_id, amount);
        }
        std::fill(pointer.begin(), pointer.end(), 0);
        std::fill(queued.begin(), queued.end(), 0);
        std::queue<int> active;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(excess[static_cast<std::size_t>(vertex)] > 0){
                active.push(vertex);
                queued[static_cast<std::size_t>(vertex)] = 1;
            }
        }
        while(!active.empty()){
            const int vertex = active.front();
            active.pop();
            queued[static_cast<std::size_t>(vertex)] = 0;
            auto& adjacent = graph[static_cast<std::size_t>(vertex)];
            while(excess[static_cast<std::size_t>(vertex)] > 0){
                auto& edge_pointer =
                    pointer[static_cast<std::size_t>(vertex)];
                while(edge_pointer < adjacent.size()){
                    const int edge_id = adjacent[edge_pointer];
                    const auto& edge =
                        residual_edges[static_cast<std::size_t>(edge_id)];
                    if(edge.capacity > 0 && reduced_cost(edge) < 0) break;
                    ++edge_pointer;
                }
                if(edge_pointer == adjacent.size()){
                    bool found = false;
                    Wide minimum_reduced_cost = 0;
                    for(int edge_id: adjacent){
                        const auto& edge = residual_edges[
                            static_cast<std::size_t>(edge_id)
                        ];
                        if(edge.capacity <= 0) continue;
                        const Wide value = reduced_cost(edge);
                        if(!found || value < minimum_reduced_cost){
                            found = true;
                            minimum_reduced_cost = value;
                        }
                    }
                    if(!found)[[unlikely]]{
                        throw std::logic_error(
                            "active vertex has no residual edge "
                            "(minimum_cost_circulation)"
                        );
                    }
                    potential[static_cast<std::size_t>(vertex)] -=
                        minimum_reduced_cost + epsilon;
                    edge_pointer = 0;
                    continue;
                }
                const int edge_id = adjacent[edge_pointer];
                auto& edge =
                    residual_edges[static_cast<std::size_t>(edge_id)];
                const long long amount = static_cast<long long>(std::min(
                    excess[static_cast<std::size_t>(vertex)],
                    static_cast<Wide>(edge.capacity)
                ));
                const int to = edge.to;
                const bool activate =
                    excess[static_cast<std::size_t>(to)] <= 0
                    && excess[static_cast<std::size_t>(to)] + amount > 0;
                excess[static_cast<std::size_t>(vertex)] -= amount;
                excess[static_cast<std::size_t>(to)] += amount;
                push(edge_id, amount);
                if(activate
                    && queued[static_cast<std::size_t>(to)] == 0){
                    active.push(to);
                    queued[static_cast<std::size_t>(to)] = 1;
                }
            }
        }
        if(epsilon == 1) break;
        epsilon >>= 1;
    }

    result.cost = 0;
    for(std::size_t index = 0; index < edges.size(); ++index){
        result.cost += static_cast<Wide>(result.flow[index])
            * edges[index].cost;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED
