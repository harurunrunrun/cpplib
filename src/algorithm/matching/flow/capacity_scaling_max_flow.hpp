#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CAPACITY_SCALING_MAX_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CAPACITY_SCALING_MAX_FLOW_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

template<std::signed_integral T>
struct CapacityScalingMaxFlowEdge{
    int from;
    int to;
    T capacity;
    T flow;
};

template<std::signed_integral T>
struct CapacityScalingMaxFlow{
    int vertex_count;
    std::vector<CapacityScalingMaxFlowEdge<T>> edges;
    std::vector<std::vector<int>> graph;

    explicit CapacityScalingMaxFlow(int vertex_count_)
        : vertex_count(vertex_count_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation "
                "(CapacityScalingMaxFlow)."
            )
            : vertex_count_),
          graph(static_cast<std::size_t>(vertex_count)){}

    int add_edge(int from, int to, T capacity){
        if(from < 0 || vertex_count <= from
            || to < 0 || vertex_count <= to || capacity < T{0})[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(CapacityScalingMaxFlow::add_edge)."
            );
        }
        int edge_id = static_cast<int>(edges.size());
        graph[static_cast<std::size_t>(from)].push_back(edge_id);
        edges.push_back({from, to, capacity, T{0}});
        graph[static_cast<std::size_t>(to)].push_back(edge_id ^ 1);
        edges.push_back({to, from, T{0}, T{0}});
        return edge_id;
    }

    T max_flow(int source, int sink){
        if(source < 0 || vertex_count <= source
            || sink < 0 || vertex_count <= sink)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(CapacityScalingMaxFlow::max_flow)."
            );
        }
        if(source == sink) return T{0};
        T maximum_residual = 0;
        for(const auto& edge: edges){
            maximum_residual = std::max(
                maximum_residual, edge.capacity - edge.flow
            );
        }
        if(maximum_residual == 0) return T{0};
        T scale = 1;
        while(scale <= maximum_residual / 2) scale *= 2;
        T result = 0;
        std::vector<int> parent_edge(
            static_cast<std::size_t>(vertex_count)
        );
        while(scale > 0){
            while(true){
                std::fill(parent_edge.begin(), parent_edge.end(), -1);
                std::queue<int> queue;
                parent_edge[static_cast<std::size_t>(source)] = -2;
                queue.push(source);
                while(!queue.empty()
                    && parent_edge[static_cast<std::size_t>(sink)] == -1){
                    int vertex = queue.front();
                    queue.pop();
                    for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                        const auto& edge =
                            edges[static_cast<std::size_t>(edge_id)];
                        if(edge.capacity - edge.flow < scale
                            || parent_edge[static_cast<std::size_t>(edge.to)]
                                != -1){
                            continue;
                        }
                        parent_edge[static_cast<std::size_t>(edge.to)] =
                            edge_id;
                        queue.push(edge.to);
                    }
                }
                if(parent_edge[static_cast<std::size_t>(sink)] == -1) break;
                T pushed = std::numeric_limits<T>::max();
                for(int vertex = sink; vertex != source; ){
                    int edge_id =
                        parent_edge[static_cast<std::size_t>(vertex)];
                    const auto& edge =
                        edges[static_cast<std::size_t>(edge_id)];
                    pushed = std::min(
                        pushed, edge.capacity - edge.flow
                    );
                    vertex = edge.from;
                }
                if(result > std::numeric_limits<T>::max() - pushed)[[unlikely]]{
                    throw std::overflow_error(
                        "CapacityScalingMaxFlow result overflows value type"
                    );
                }
                for(int vertex = sink; vertex != source; ){
                    int edge_id =
                        parent_edge[static_cast<std::size_t>(vertex)];
                    auto& edge = edges[static_cast<std::size_t>(edge_id)];
                    edge.flow += pushed;
                    edges[static_cast<std::size_t>(edge_id ^ 1)].flow -= pushed;
                    vertex = edge.from;
                }
                result += pushed;
            }
            scale /= 2;
        }
        return result;
    }

    [[nodiscard]] std::vector<char> min_cut(int source) const{
        if(source < 0 || vertex_count <= source)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(CapacityScalingMaxFlow::min_cut)."
            );
        }
        std::vector<char> seen(static_cast<std::size_t>(vertex_count), 0);
        std::vector<int> stack{source};
        seen[static_cast<std::size_t>(source)] = 1;
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                const auto& edge =
                    edges[static_cast<std::size_t>(edge_id)];
                if(edge.capacity - edge.flow <= 0
                    || seen[static_cast<std::size_t>(edge.to)] != 0){
                    continue;
                }
                seen[static_cast<std::size_t>(edge.to)] = 1;
                stack.push_back(edge.to);
            }
        }
        return seen;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CAPACITY_SCALING_MAX_FLOW_HPP_INCLUDED
