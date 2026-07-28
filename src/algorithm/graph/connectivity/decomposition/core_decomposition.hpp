#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CORE_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CORE_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct CoreDecompositionResult{
    std::vector<int> core_number;
    std::vector<int> degeneracy_order;
    int degeneracy;
};

inline CoreDecompositionResult core_decomposition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (core_decomposition)."
        );
    }
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<int> degree(static_cast<std::size_t>(vertex_count), 0);
    for(const auto& [u, v]: edges){
        if(u < 0 || vertex_count <= u || v < 0 || vertex_count <= v)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (core_decomposition)."
            );
        }
        if(u == v) continue;
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
        ++degree[static_cast<std::size_t>(u)];
        ++degree[static_cast<std::size_t>(v)];
    }

    int maximum_degree = 0;
    for(int value: degree) maximum_degree = std::max(maximum_degree, value);
    std::vector<int> bin(static_cast<std::size_t>(maximum_degree + 1), 0);
    for(int value: degree) ++bin[static_cast<std::size_t>(value)];
    int begin = 0;
    for(int value = 0; value <= maximum_degree; ++value){
        int count = bin[static_cast<std::size_t>(value)];
        bin[static_cast<std::size_t>(value)] = begin;
        begin += count;
    }

    std::vector<int> position(static_cast<std::size_t>(vertex_count));
    std::vector<int> order(static_cast<std::size_t>(vertex_count));
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        int& next = bin[static_cast<std::size_t>(
            degree[static_cast<std::size_t>(vertex)]
        )];
        position[static_cast<std::size_t>(vertex)] = next;
        order[static_cast<std::size_t>(next++)] = vertex;
    }
    for(int value = maximum_degree; value >= 1; --value){
        bin[static_cast<std::size_t>(value)] =
            bin[static_cast<std::size_t>(value - 1)];
    }
    if(!bin.empty()) bin[0] = 0;

    std::vector<int> core(static_cast<std::size_t>(vertex_count), 0);
    int degeneracy = 0;
    for(int index = 0; index < vertex_count; ++index){
        int vertex = order[static_cast<std::size_t>(index)];
        const int vertex_degree = degree[static_cast<std::size_t>(vertex)];
        core[static_cast<std::size_t>(vertex)] = vertex_degree;
        degeneracy = std::max(degeneracy, vertex_degree);
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(degree[static_cast<std::size_t>(to)] <= vertex_degree) continue;
            int to_degree = degree[static_cast<std::size_t>(to)];
            int to_position = position[static_cast<std::size_t>(to)];
            int first_position = bin[static_cast<std::size_t>(to_degree)];
            int first_vertex = order[static_cast<std::size_t>(first_position)];
            if(to != first_vertex){
                std::swap(
                    order[static_cast<std::size_t>(to_position)],
                    order[static_cast<std::size_t>(first_position)]
                );
                position[static_cast<std::size_t>(to)] = first_position;
                position[static_cast<std::size_t>(first_vertex)] = to_position;
            }
            ++bin[static_cast<std::size_t>(to_degree)];
            --degree[static_cast<std::size_t>(to)];
        }
    }
    return {std::move(core), std::move(order), degeneracy};
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CORE_DECOMPOSITION_HPP_INCLUDED
