#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_PUSH_RELABEL_MAX_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_PUSH_RELABEL_MAX_FLOW_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <limits>
#include <list>
#include <stdexcept>
#include <vector>

template<std::signed_integral T>
struct PushRelabelEdge{
    int from;
    int to;
    T capacity;
    T flow;
};

template<std::signed_integral T>
struct PushRelabel{
    int vertex_count;
    std::vector<PushRelabelEdge<T>> edges;
    std::vector<std::vector<int>> graph;

    explicit PushRelabel(int vertex_count_)
        : vertex_count(vertex_count_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (PushRelabel)."
            )
            : vertex_count_),
          graph(static_cast<std::size_t>(vertex_count)){}

    int add_edge(int from, int to, T capacity){
        if(from < 0 || vertex_count <= from
            || to < 0 || vertex_count <= to || capacity < T{0})[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(PushRelabel::add_edge)."
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
                "(PushRelabel::max_flow)."
            );
        }
        if(source == sink) return T{0};
        T source_capacity = 0;
        for(int edge_id: graph[static_cast<std::size_t>(source)]){
            const auto& edge = edges[static_cast<std::size_t>(edge_id)];
            T residual = edge.capacity - edge.flow;
            if(edge.to == source || residual <= 0) continue;
            if(source_capacity > std::numeric_limits<T>::max() - residual){
                throw std::overflow_error(
                    "PushRelabel source capacity overflows value type"
                );
            }
            source_capacity += residual;
        }
        std::vector<T> excess(static_cast<std::size_t>(vertex_count), T{0});
        std::vector<int> height(static_cast<std::size_t>(vertex_count), 0);
        std::vector<std::size_t> current(
            static_cast<std::size_t>(vertex_count), 0
        );
        height[static_cast<std::size_t>(source)] = vertex_count;
        for(int edge_id: graph[static_cast<std::size_t>(source)]){
            auto& edge = edges[static_cast<std::size_t>(edge_id)];
            T pushed = edge.capacity - edge.flow;
            if(edge.to == source || pushed <= 0) continue;
            edge.flow += pushed;
            edges[static_cast<std::size_t>(edge_id ^ 1)].flow -= pushed;
            excess[static_cast<std::size_t>(source)] -= pushed;
            excess[static_cast<std::size_t>(edge.to)] += pushed;
        }
        auto push = [&](int vertex, int edge_id){
            auto& edge = edges[static_cast<std::size_t>(edge_id)];
            T pushed = std::min(
                excess[static_cast<std::size_t>(vertex)],
                edge.capacity - edge.flow
            );
            if(pushed <= 0) return;
            edge.flow += pushed;
            edges[static_cast<std::size_t>(edge_id ^ 1)].flow -= pushed;
            excess[static_cast<std::size_t>(vertex)] -= pushed;
            excess[static_cast<std::size_t>(edge.to)] += pushed;
        };
        auto relabel = [&](int vertex){
            int next_height = std::numeric_limits<int>::max();
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
                if(edge.capacity - edge.flow > 0){
                    next_height = std::min(
                        next_height,
                        height[static_cast<std::size_t>(edge.to)]
                    );
                }
            }
            if(next_height == std::numeric_limits<int>::max())[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: active vertex has no residual "
                    "edge (PushRelabel::max_flow)."
                );
            }
            height[static_cast<std::size_t>(vertex)] = next_height + 1;
        };
        auto discharge = [&](int vertex){
            while(excess[static_cast<std::size_t>(vertex)] > 0){
                auto& cursor = current[static_cast<std::size_t>(vertex)];
                if(cursor == graph[static_cast<std::size_t>(vertex)].size()){
                    relabel(vertex);
                    cursor = 0;
                    continue;
                }
                int edge_id =
                    graph[static_cast<std::size_t>(vertex)][cursor];
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
                if(edge.capacity - edge.flow > 0
                    && height[static_cast<std::size_t>(vertex)]
                        == height[static_cast<std::size_t>(edge.to)] + 1){
                    push(vertex, edge_id);
                }else{
                    ++cursor;
                }
            }
        };
        std::list<int> vertices;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(vertex != source && vertex != sink) vertices.push_back(vertex);
        }
        for(auto iterator = vertices.begin(); iterator != vertices.end(); ){
            int vertex = *iterator;
            int old_height = height[static_cast<std::size_t>(vertex)];
            discharge(vertex);
            if(height[static_cast<std::size_t>(vertex)] > old_height){
                vertices.erase(iterator++);
                vertices.push_front(vertex);
                iterator = vertices.begin();
            }else{
                ++iterator;
            }
        }
        return excess[static_cast<std::size_t>(sink)];
    }

    [[nodiscard]] std::vector<char> min_cut(int source) const{
        if(source < 0 || vertex_count <= source)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(PushRelabel::min_cut)."
            );
        }
        std::vector<char> seen(static_cast<std::size_t>(vertex_count), 0);
        std::vector<int> stack{source};
        seen[static_cast<std::size_t>(source)] = 1;
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
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

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_PUSH_RELABEL_MAX_FLOW_HPP_INCLUDED
