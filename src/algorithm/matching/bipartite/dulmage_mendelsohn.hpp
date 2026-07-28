#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_DULMAGE_MENDELSOHN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_DULMAGE_MENDELSOHN_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "bipartite_matching.hpp"

enum class DulmageMendelsohnPart{
    left_deficient,
    balanced,
    right_deficient
};

struct DulmageMendelsohnResult{
    int matching_size;
    int balanced_component_count;
    std::vector<int> left_match;
    std::vector<int> right_match;
    std::vector<DulmageMendelsohnPart> left_part;
    std::vector<DulmageMendelsohnPart> right_part;
    std::vector<int> left_component;
    std::vector<int> right_component;
};

struct DulmageMendelsohnDecomposition{
    int left_size;
    int right_size;
    std::vector<std::pair<int, int>> edges;

    DulmageMendelsohnDecomposition(int left_size_, int right_size_)
        : left_size(left_size_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation "
                "(DulmageMendelsohnDecomposition)."
            )
            : left_size_),
          right_size(right_size_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation "
                "(DulmageMendelsohnDecomposition)."
            )
            : right_size_){}

    void add_edge(int left, int right){
        if(left < 0 || left_size <= left || right < 0 || right_size <= right)
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DulmageMendelsohnDecomposition::add_edge)."
            );
        }
        edges.emplace_back(left, right);
    }

    DulmageMendelsohnResult solve() const{
        BipartiteMatching matching(left_size, right_size);
        for(auto [left, right]: edges) matching.add_edge(left, right);
        BipartiteMatchingResult maximum = matching.solve();

        const int vertex_count = left_size + right_size;
        std::vector<std::vector<int>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        std::vector<std::vector<int>> reverse_graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(auto [left, right]: edges){
            int right_vertex = left_size + right;
            int from = left;
            int to = right_vertex;
            if(maximum.left_match[static_cast<std::size_t>(left)] == right){
                from = right_vertex;
                to = left;
            }
            graph[static_cast<std::size_t>(from)].push_back(to);
            reverse_graph[static_cast<std::size_t>(to)].push_back(from);
        }

        auto reachable = [&](const std::vector<std::vector<int>>& adjacency,
            const std::vector<int>& sources){
            std::vector<unsigned char> seen(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<int> stack = sources;
            for(int source: stack) seen[static_cast<std::size_t>(source)] = 1;
            while(!stack.empty()){
                int vertex = stack.back();
                stack.pop_back();
                for(int to: adjacency[static_cast<std::size_t>(vertex)]){
                    if(seen[static_cast<std::size_t>(to)] != 0) continue;
                    seen[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
            return seen;
        };
        std::vector<int> free_left;
        std::vector<int> free_right;
        for(int left = 0; left < left_size; ++left){
            if(maximum.left_match[static_cast<std::size_t>(left)] == -1){
                free_left.push_back(left);
            }
        }
        for(int right = 0; right < right_size; ++right){
            if(maximum.right_match[static_cast<std::size_t>(right)] == -1){
                free_right.push_back(left_size + right);
            }
        }
        std::vector<unsigned char> from_free_left =
            reachable(graph, free_left);
        std::vector<unsigned char> to_free_right =
            reachable(reverse_graph, free_right);

        std::vector<unsigned char> balanced(
            static_cast<std::size_t>(vertex_count), 0
        );
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            balanced[static_cast<std::size_t>(vertex)] =
                from_free_left[static_cast<std::size_t>(vertex)] == 0
                && to_free_right[static_cast<std::size_t>(vertex)] == 0;
        }

        std::vector<unsigned char> visited(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> order;
        order.reserve(static_cast<std::size_t>(vertex_count));
        for(int start = 0; start < vertex_count; ++start){
            if(balanced[static_cast<std::size_t>(start)] == 0
                || visited[static_cast<std::size_t>(start)] != 0) continue;
            std::vector<std::pair<int, std::size_t>> stack{{start, 0}};
            visited[static_cast<std::size_t>(start)] = 1;
            while(!stack.empty()){
                int vertex = stack.back().first;
                std::size_t& index = stack.back().second;
                const auto& adjacency =
                    graph[static_cast<std::size_t>(vertex)];
                while(index < adjacency.size()
                    && (balanced[static_cast<std::size_t>(adjacency[index])]
                            == 0
                        || visited[static_cast<std::size_t>(
                            adjacency[index])] != 0)){
                    ++index;
                }
                if(index == adjacency.size()){
                    order.push_back(vertex);
                    stack.pop_back();
                    continue;
                }
                int to = adjacency[index++];
                visited[static_cast<std::size_t>(to)] = 1;
                stack.emplace_back(to, 0);
            }
        }

        std::vector<int> component(
            static_cast<std::size_t>(vertex_count), -1
        );
        int component_count = 0;
        for(auto iterator = order.rbegin(); iterator != order.rend();
            ++iterator){
            int start = *iterator;
            if(component[static_cast<std::size_t>(start)] != -1) continue;
            component[static_cast<std::size_t>(start)] = component_count;
            std::vector<int> stack{start};
            while(!stack.empty()){
                int vertex = stack.back();
                stack.pop_back();
                for(int to:
                    reverse_graph[static_cast<std::size_t>(vertex)]){
                    if(balanced[static_cast<std::size_t>(to)] == 0
                        || component[static_cast<std::size_t>(to)] != -1){
                        continue;
                    }
                    component[static_cast<std::size_t>(to)] =
                        component_count;
                    stack.push_back(to);
                }
            }
            ++component_count;
        }

        DulmageMendelsohnResult result;
        result.matching_size = maximum.size;
        result.balanced_component_count = component_count;
        result.left_match = std::move(maximum.left_match);
        result.right_match = std::move(maximum.right_match);
        result.left_part.resize(static_cast<std::size_t>(left_size));
        result.right_part.resize(static_cast<std::size_t>(right_size));
        result.left_component.resize(
            static_cast<std::size_t>(left_size), -1
        );
        result.right_component.resize(
            static_cast<std::size_t>(right_size), -1
        );
        auto part = [&](int vertex){
            if(from_free_left[static_cast<std::size_t>(vertex)] != 0){
                return DulmageMendelsohnPart::left_deficient;
            }
            if(to_free_right[static_cast<std::size_t>(vertex)] != 0){
                return DulmageMendelsohnPart::right_deficient;
            }
            return DulmageMendelsohnPart::balanced;
        };
        for(int left = 0; left < left_size; ++left){
            result.left_part[static_cast<std::size_t>(left)] = part(left);
            result.left_component[static_cast<std::size_t>(left)] =
                component[static_cast<std::size_t>(left)];
        }
        for(int right = 0; right < right_size; ++right){
            int vertex = left_size + right;
            result.right_part[static_cast<std::size_t>(right)] = part(vertex);
            result.right_component[static_cast<std::size_t>(right)] =
                component[static_cast<std::size_t>(vertex)];
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_DULMAGE_MENDELSOHN_HPP_INCLUDED
