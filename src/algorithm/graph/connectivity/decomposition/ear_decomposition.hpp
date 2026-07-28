#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_EAR_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_EAR_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "lowlink.hpp"

struct EarDecompositionResult{
    std::vector<std::vector<int>> ears;
    std::vector<std::vector<int>> ear_edge_ids;
};

inline std::optional<EarDecompositionResult> ear_decomposition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (ear_decomposition)."
        );
    }
    std::vector<std::vector<std::pair<int, int>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
        auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
        if(left < 0 || vertex_count <= left
            || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(ear_decomposition)."
            );
        }
        if(left == right) return std::nullopt;
        graph[static_cast<std::size_t>(left)].emplace_back(right, edge_id);
        graph[static_cast<std::size_t>(right)].emplace_back(left, edge_id);
    }
    if(vertex_count < 2 || edges.empty()) return std::nullopt;
    std::vector<unsigned char> connected(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<int> connectivity_stack{0};
    connected[0] = 1;
    while(!connectivity_stack.empty()){
        int vertex = connectivity_stack.back();
        connectivity_stack.pop_back();
        for(auto [to, edge_id]: graph[static_cast<std::size_t>(vertex)]){
            static_cast<void>(edge_id);
            if(connected[static_cast<std::size_t>(to)] == 0){
                connected[static_cast<std::size_t>(to)] = 1;
                connectivity_stack.push_back(to);
            }
        }
    }
    for(unsigned char value: connected){
        if(value == 0) return std::nullopt;
    }
    if(!lowlink(vertex_count, edges).articulation.empty()){
        return std::nullopt;
    }

    std::vector<int> order(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> parent_edge(static_cast<std::size_t>(vertex_count), -1);
    int timer = 0;
    int cycle_from = -1;
    int cycle_to = -1;
    int closing_edge = -1;
    std::function<void(int, int)> find_cycle =
        [&](int vertex, int incoming_edge) -> void {
        order[static_cast<std::size_t>(vertex)] = timer++;
        for(auto [to, edge_id]: graph[static_cast<std::size_t>(vertex)]){
            if(closing_edge != -1) return;
            if(edge_id == incoming_edge) continue;
            if(order[static_cast<std::size_t>(to)] == -1){
                parent[static_cast<std::size_t>(to)] = vertex;
                parent_edge[static_cast<std::size_t>(to)] = edge_id;
                find_cycle(to, edge_id);
            }else if(order[static_cast<std::size_t>(to)]
                < order[static_cast<std::size_t>(vertex)]){
                cycle_from = vertex;
                cycle_to = to;
                closing_edge = edge_id;
                return;
            }
        }
    };
    find_cycle(0, -1);
    if(closing_edge == -1) return std::nullopt;
    std::vector<int> reversed_vertices;
    std::vector<int> reversed_edges;
    for(int vertex = cycle_from; vertex != cycle_to;
        vertex = parent[static_cast<std::size_t>(vertex)]){
        reversed_vertices.push_back(vertex);
        reversed_edges.push_back(
            parent_edge[static_cast<std::size_t>(vertex)]
        );
    }
    std::reverse(reversed_vertices.begin(), reversed_vertices.end());
    std::reverse(reversed_edges.begin(), reversed_edges.end());
    std::vector<int> initial_cycle{cycle_to};
    initial_cycle.insert(
        initial_cycle.end(),
        reversed_vertices.begin(),
        reversed_vertices.end()
    );
    reversed_edges.push_back(closing_edge);
    EarDecompositionResult result;
    result.ears.push_back(std::move(initial_cycle));
    result.ear_edge_ids.push_back(std::move(reversed_edges));

    std::vector<unsigned char> included(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<unsigned char> used_edge(edges.size(), 0);
    for(int vertex: result.ears[0]){
        included[static_cast<std::size_t>(vertex)] = 1;
    }
    for(int edge_id: result.ear_edge_ids[0]){
        used_edge[static_cast<std::size_t>(edge_id)] = 1;
    }
    while(true){
        int component_start = -1;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(included[static_cast<std::size_t>(vertex)] == 0){
                component_start = vertex;
                break;
            }
        }
        if(component_start == -1) break;
        std::vector<unsigned char> in_component(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> component;
        std::vector<std::tuple<int, int, int>> boundary;
        std::queue<int> queue;
        in_component[static_cast<std::size_t>(component_start)] = 1;
        queue.push(component_start);
        while(!queue.empty()){
            int vertex = queue.front();
            queue.pop();
            component.push_back(vertex);
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(vertex)]){
                if(included[static_cast<std::size_t>(to)] != 0){
                    boundary.emplace_back(to, vertex, edge_id);
                }else if(in_component[static_cast<std::size_t>(to)] == 0){
                    in_component[static_cast<std::size_t>(to)] = 1;
                    queue.push(to);
                }
            }
        }
        int first_boundary = -1;
        int second_boundary = -1;
        for(int index = 0; index < static_cast<int>(boundary.size()); ++index){
            if(first_boundary == -1){
                first_boundary = index;
            }else if(std::get<0>(boundary[static_cast<std::size_t>(index)])
                != std::get<0>(boundary[
                    static_cast<std::size_t>(first_boundary)
                ])){
                second_boundary = index;
                break;
            }
        }
        if(second_boundary == -1) return std::nullopt;
        auto [left_endpoint, left_inside, left_edge] =
            boundary[static_cast<std::size_t>(first_boundary)];
        auto [right_endpoint, right_inside, right_edge] =
            boundary[static_cast<std::size_t>(second_boundary)];
        std::vector<int> path_parent(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::vector<int> path_parent_edge(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::queue<int> path_queue;
        path_parent[static_cast<std::size_t>(left_inside)] = left_inside;
        path_queue.push(left_inside);
        while(!path_queue.empty()
            && path_parent[static_cast<std::size_t>(right_inside)] == -1){
            int vertex = path_queue.front();
            path_queue.pop();
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(vertex)]){
                if(in_component[static_cast<std::size_t>(to)] == 0
                    || path_parent[static_cast<std::size_t>(to)] != -1){
                    continue;
                }
                path_parent[static_cast<std::size_t>(to)] = vertex;
                path_parent_edge[static_cast<std::size_t>(to)] = edge_id;
                path_queue.push(to);
            }
        }
        if(path_parent[static_cast<std::size_t>(right_inside)] == -1){
            return std::nullopt;
        }
        std::vector<int> internal_vertices;
        std::vector<int> internal_edges;
        for(int vertex = right_inside; vertex != left_inside;
            vertex = path_parent[static_cast<std::size_t>(vertex)]){
            internal_vertices.push_back(vertex);
            internal_edges.push_back(
                path_parent_edge[static_cast<std::size_t>(vertex)]
            );
        }
        internal_vertices.push_back(left_inside);
        std::reverse(internal_vertices.begin(), internal_vertices.end());
        std::reverse(internal_edges.begin(), internal_edges.end());
        std::vector<int> ear{left_endpoint};
        ear.insert(
            ear.end(), internal_vertices.begin(), internal_vertices.end()
        );
        ear.push_back(right_endpoint);
        std::vector<int> ear_edges{left_edge};
        ear_edges.insert(
            ear_edges.end(), internal_edges.begin(), internal_edges.end()
        );
        ear_edges.push_back(right_edge);
        for(int vertex: internal_vertices){
            included[static_cast<std::size_t>(vertex)] = 1;
        }
        for(int edge_id: ear_edges){
            used_edge[static_cast<std::size_t>(edge_id)] = 1;
        }
        result.ears.push_back(std::move(ear));
        result.ear_edge_ids.push_back(std::move(ear_edges));
    }
    for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
        if(used_edge[static_cast<std::size_t>(edge_id)] != 0) continue;
        auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
        result.ears.push_back({left, right});
        result.ear_edge_ids.push_back({edge_id});
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_EAR_DECOMPOSITION_HPP_INCLUDED
