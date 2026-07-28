#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_DIRECTED_CHINESE_POSTMAN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_DIRECTED_CHINESE_POSTMAN_HPP_INCLUDED

#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../matching/flow/min_cost_flow.hpp"

struct DirectedChinesePostmanEdge{
    int from;
    int to;
    long long cost;
};

struct DirectedChinesePostmanAugmentation{
    int from;
    int to;
    long long multiplicity;
    long long distance;
};

struct DirectedChinesePostmanResult{
    long long cost;
    std::vector<DirectedChinesePostmanAugmentation> augmentations;
};

inline std::optional<DirectedChinesePostmanResult> directed_chinese_postman(
    int vertex_count,
    const std::vector<DirectedChinesePostmanEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(directed_chinese_postman)."
        );
    }
    using Wide = __int128;
    constexpr Wide infinity = Wide{1} << 120;
    std::vector<std::vector<std::pair<int, long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<std::vector<int>> reverse_graph(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<long long> balance(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<unsigned char> incident(
        static_cast<std::size_t>(vertex_count), 0
    );
    Wide base_cost = 0;
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.cost < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(directed_chinese_postman)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].emplace_back(
            edge.to, edge.cost
        );
        reverse_graph[static_cast<std::size_t>(edge.to)].push_back(edge.from);
        ++balance[static_cast<std::size_t>(edge.from)];
        --balance[static_cast<std::size_t>(edge.to)];
        incident[static_cast<std::size_t>(edge.from)] = 1;
        incident[static_cast<std::size_t>(edge.to)] = 1;
        base_cost += static_cast<Wide>(edge.cost);
    }
    if(edges.empty()) return DirectedChinesePostmanResult{0, {}};
    int root = -1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(incident[static_cast<std::size_t>(vertex)] != 0){
            root = vertex;
            break;
        }
    }
    for(int reverse = 0; reverse < 2; ++reverse){
        std::vector<unsigned char> seen(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> stack{root};
        seen[static_cast<std::size_t>(root)] = 1;
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            if(reverse == 0){
                for(auto [to, cost]: graph[static_cast<std::size_t>(vertex)]){
                    static_cast<void>(cost);
                    if(seen[static_cast<std::size_t>(to)] == 0){
                        seen[static_cast<std::size_t>(to)] = 1;
                        stack.push_back(to);
                    }
                }
            }else{
                for(int to: reverse_graph[static_cast<std::size_t>(vertex)]){
                    if(seen[static_cast<std::size_t>(to)] == 0){
                        seen[static_cast<std::size_t>(to)] = 1;
                        stack.push_back(to);
                    }
                }
            }
        }
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(incident[static_cast<std::size_t>(vertex)] != 0
                && seen[static_cast<std::size_t>(vertex)] == 0){
                return std::nullopt;
            }
        }
    }

    std::vector<int> supplies;
    std::vector<int> demands;
    long long required = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        long long value = balance[static_cast<std::size_t>(vertex)];
        if(value < 0){
            supplies.push_back(vertex);
            required -= value;
        }else if(value > 0){
            demands.push_back(vertex);
        }
    }
    if(required == 0){
        if(base_cost > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "directed_chinese_postman cost overflows long long"
            );
        }
        return DirectedChinesePostmanResult{
            static_cast<long long>(base_cost), {}
        };
    }
    std::vector<std::vector<Wide>> distance(
        supplies.size(),
        std::vector<Wide>(static_cast<std::size_t>(vertex_count), infinity)
    );
    using QueueEntry = std::pair<Wide, int>;
    for(std::size_t source_index = 0;
        source_index < supplies.size();
        ++source_index){
        int source = supplies[source_index];
        auto& current = distance[source_index];
        current[static_cast<std::size_t>(source)] = 0;
        std::priority_queue<
            QueueEntry,
            std::vector<QueueEntry>,
            std::greater<QueueEntry>
        > queue;
        queue.push({0, source});
        while(!queue.empty()){
            auto [value, vertex] = queue.top();
            queue.pop();
            if(value != current[static_cast<std::size_t>(vertex)]) continue;
            for(auto [to, cost]: graph[static_cast<std::size_t>(vertex)]){
                Wide candidate = value + static_cast<Wide>(cost);
                if(candidate < current[static_cast<std::size_t>(to)]){
                    current[static_cast<std::size_t>(to)] = candidate;
                    queue.push({candidate, to});
                }
            }
        }
    }

    const int source_node =
        static_cast<int>(supplies.size() + demands.size());
    const int sink_node = source_node + 1;
    MinCostFlow<Wide> flow(sink_node + 1);
    for(std::size_t index = 0; index < supplies.size(); ++index){
        int vertex = supplies[index];
        flow.add_edge(
            source_node,
            static_cast<int>(index),
            -static_cast<Wide>(balance[static_cast<std::size_t>(vertex)]),
            0
        );
    }
    for(std::size_t index = 0; index < demands.size(); ++index){
        int vertex = demands[index];
        flow.add_edge(
            static_cast<int>(supplies.size() + index),
            sink_node,
            static_cast<Wide>(balance[static_cast<std::size_t>(vertex)]),
            0
        );
    }
    struct TransportEdge{
        int edge_id;
        int from;
        int to;
        Wide distance;
    };
    std::vector<TransportEdge> transport_edges;
    for(std::size_t left = 0; left < supplies.size(); ++left){
        for(std::size_t right = 0; right < demands.size(); ++right){
            Wide value = distance[left][static_cast<std::size_t>(
                demands[right]
            )];
            if(value == infinity) continue;
            int edge_id = flow.add_edge(
                static_cast<int>(left),
                static_cast<int>(supplies.size() + right),
                static_cast<Wide>(required),
                value
            );
            transport_edges.push_back({
                edge_id, supplies[left], demands[right], value
            });
        }
    }
    MinCostFlowResult<Wide> correction = flow.min_cost_flow(
        source_node,
        sink_node,
        static_cast<Wide>(required),
        infinity
    );
    if(correction.flow != static_cast<Wide>(required)) return std::nullopt;
    Wide total_cost = base_cost + correction.cost;
    if(total_cost > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "directed_chinese_postman cost overflows long long"
        );
    }
    DirectedChinesePostmanResult result{
        static_cast<long long>(total_cost), {}
    };
    for(const auto& transport: transport_edges){
        Wide multiplicity =
            flow.edges[static_cast<std::size_t>(transport.edge_id)].flow;
        if(multiplicity == 0) continue;
        if(transport.distance > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "directed_chinese_postman distance overflows long long"
            );
        }
        result.augmentations.push_back({
            transport.from,
            transport.to,
            static_cast<long long>(multiplicity),
            static_cast<long long>(transport.distance)
        });
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_DIRECTED_CHINESE_POSTMAN_HPP_INCLUDED
