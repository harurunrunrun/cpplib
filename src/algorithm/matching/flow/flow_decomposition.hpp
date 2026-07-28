#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_FLOW_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_FLOW_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct FlowDecompositionEdge{
    int from;
    int to;
    long long flow;
};

struct FlowDecompositionComponent{
    long long amount;
    bool is_cycle;
    std::vector<int> edge_ids;
};

inline std::vector<FlowDecompositionComponent> flow_decomposition(
    int vertex_count,
    const std::vector<FlowDecompositionEdge>& edges,
    int source,
    int sink
){
    if(vertex_count < 0 || source < 0 || vertex_count <= source
        || sink < 0 || vertex_count <= sink || source == sink)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (flow_decomposition)."
        );
    }
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<__int128> balance(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<long long> remaining(edges.size());
    for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
        const auto& edge = edges[static_cast<std::size_t>(edge_id)];
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.flow < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(flow_decomposition)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].push_back(edge_id);
        balance[static_cast<std::size_t>(edge.from)] += edge.flow;
        balance[static_cast<std::size_t>(edge.to)] -= edge.flow;
        remaining[static_cast<std::size_t>(edge_id)] = edge.flow;
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(vertex != source && vertex != sink
            && balance[static_cast<std::size_t>(vertex)] != 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: flow conservation violation "
                "(flow_decomposition)."
            );
        }
    }
    if(balance[static_cast<std::size_t>(source)] < 0
        || balance[static_cast<std::size_t>(sink)]
            != -balance[static_cast<std::size_t>(source)])[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: source/sink imbalance "
            "(flow_decomposition)."
        );
    }
    std::vector<FlowDecompositionComponent> result;
    while(balance[static_cast<std::size_t>(source)] > 0){
        std::vector<int> parent_edge(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::queue<int> queue;
        parent_edge[static_cast<std::size_t>(source)] = -2;
        queue.push(source);
        while(!queue.empty()
            && parent_edge[static_cast<std::size_t>(sink)] == -1){
            int vertex = queue.front();
            queue.pop();
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
                if(remaining[static_cast<std::size_t>(edge_id)] == 0
                    || parent_edge[static_cast<std::size_t>(edge.to)] != -1){
                    continue;
                }
                parent_edge[static_cast<std::size_t>(edge.to)] = edge_id;
                queue.push(edge.to);
            }
        }
        if(parent_edge[static_cast<std::size_t>(sink)] == -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: no positive-flow source-sink path "
                "(flow_decomposition)."
            );
        }
        std::vector<int> path;
        long long amount = std::numeric_limits<long long>::max();
        for(int vertex = sink; vertex != source; ){
            int edge_id = parent_edge[static_cast<std::size_t>(vertex)];
            path.push_back(edge_id);
            amount = std::min(
                amount, remaining[static_cast<std::size_t>(edge_id)]
            );
            vertex = edges[static_cast<std::size_t>(edge_id)].from;
        }
        if(balance[static_cast<std::size_t>(source)] < amount){
            amount = static_cast<long long>(
                balance[static_cast<std::size_t>(source)]
            );
        }
        std::reverse(path.begin(), path.end());
        for(int edge_id: path){
            remaining[static_cast<std::size_t>(edge_id)] -= amount;
        }
        balance[static_cast<std::size_t>(source)] -= amount;
        balance[static_cast<std::size_t>(sink)] += amount;
        result.push_back({amount, false, std::move(path)});
    }
    while(true){
        int first_edge = -1;
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
            if(remaining[static_cast<std::size_t>(edge_id)] > 0){
                first_edge = edge_id;
                break;
            }
        }
        if(first_edge == -1) break;
        std::vector<int> first_position(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::vector<int> walk_edges;
        int vertex = edges[static_cast<std::size_t>(first_edge)].from;
        while(first_position[static_cast<std::size_t>(vertex)] == -1){
            first_position[static_cast<std::size_t>(vertex)] =
                static_cast<int>(walk_edges.size());
            int next_edge = -1;
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                if(remaining[static_cast<std::size_t>(edge_id)] > 0){
                    next_edge = edge_id;
                    break;
                }
            }
            if(next_edge == -1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: residual flow is not a "
                    "circulation (flow_decomposition)."
                );
            }
            walk_edges.push_back(next_edge);
            vertex = edges[static_cast<std::size_t>(next_edge)].to;
        }
        int cycle_begin =
            first_position[static_cast<std::size_t>(vertex)];
        std::vector<int> cycle(
            walk_edges.begin() + cycle_begin, walk_edges.end()
        );
        long long amount = std::numeric_limits<long long>::max();
        for(int edge_id: cycle){
            amount = std::min(
                amount, remaining[static_cast<std::size_t>(edge_id)]
            );
        }
        for(int edge_id: cycle){
            remaining[static_cast<std::size_t>(edge_id)] -= amount;
        }
        result.push_back({amount, true, std::move(cycle)});
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_FLOW_DECOMPOSITION_HPP_INCLUDED
