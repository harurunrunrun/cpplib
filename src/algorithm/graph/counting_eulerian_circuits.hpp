#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_COUNTING_EULERIAN_CIRCUITS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_COUNTING_EULERIAN_CIRCUITS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "matrix_tree_theorem.hpp"

template<class T, std::size_t MAX_VERTEX>
T count_eulerian_circuits(
    int vertex_count,
    const std::vector<std::pair<int, int>>& directed_edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(count_eulerian_circuits).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    if(directed_edges.empty()) return T();

    std::vector<int> indegree(static_cast<std::size_t>(vertex_count));
    std::vector<int> outdegree(static_cast<std::size_t>(vertex_count));
    for(const auto& [from, to]: directed_edges){
        if(from < 0 || vertex_count <= from ||
           to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::runtime_error(message);
        }
        ++outdegree[static_cast<std::size_t>(from)];
        ++indegree[static_cast<std::size_t>(to)];
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(indegree[static_cast<std::size_t>(vertex)] !=
           outdegree[static_cast<std::size_t>(vertex)]){
            return T();
        }
    }

    std::vector<int> compressed(static_cast<std::size_t>(vertex_count), -1);
    int active_count = 0;
    int maximum_degree = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const int degree = outdegree[static_cast<std::size_t>(vertex)];
        maximum_degree = degree > maximum_degree ? degree : maximum_degree;
        if(degree != 0){
            compressed[static_cast<std::size_t>(vertex)] = active_count++;
        }
    }
    if(static_cast<std::size_t>(active_count) > MAX_VERTEX)[[unlikely]]{
        throw std::runtime_error(message);
    }

    std::vector<MatrixTreeEdge<T>> tree_edges;
    tree_edges.reserve(directed_edges.size());
    for(const auto& [from, to]: directed_edges){
        tree_edges.push_back({
            compressed[static_cast<std::size_t>(from)],
            compressed[static_cast<std::size_t>(to)],
            T(1)
        });
    }
    const int root = compressed[
        static_cast<std::size_t>(directed_edges.front().first)
    ];
    T answer = count_weighted_directed_in_arborescences<T, MAX_VERTEX>(
        active_count, root, tree_edges
    );

    std::vector<T> factorial(static_cast<std::size_t>(maximum_degree + 1));
    factorial[0] = T(1);
    for(int value = 1; value <= maximum_degree; ++value){
        factorial[static_cast<std::size_t>(value)] =
            factorial[static_cast<std::size_t>(value - 1)] * T(value);
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const int degree = outdegree[static_cast<std::size_t>(vertex)];
        if(degree != 0){
            answer *= factorial[static_cast<std::size_t>(degree - 1)];
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_COUNTING_EULERIAN_CIRCUITS_HPP_INCLUDED
