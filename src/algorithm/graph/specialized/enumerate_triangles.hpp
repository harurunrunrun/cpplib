#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ENUMERATE_TRIANGLES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ENUMERATE_TRIANGLES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Callback>
long long enumerate_triangles(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    Callback&& callback
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (enumerate_triangles vertex_count)."
        );
    }
    std::vector<int> degree(static_cast<std::size_t>(vertex_count), 0);
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left || right < 0 || vertex_count <= right ||
           left == right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid edge (enumerate_triangles)."
            );
        }
        ++degree[static_cast<std::size_t>(left)];
        ++degree[static_cast<std::size_t>(right)];
    }

    const auto less_vertex = [&](int left, int right){
        if(degree[static_cast<std::size_t>(left)] !=
           degree[static_cast<std::size_t>(right)]){
            return degree[static_cast<std::size_t>(left)] <
                degree[static_cast<std::size_t>(right)];
        }
        return left < right;
    };

    std::vector<std::vector<int>> outgoing(static_cast<std::size_t>(vertex_count));
    for(const auto& [edge_left, edge_right]: edges){
        int left = edge_left;
        int right = edge_right;
        if(!less_vertex(left, right)) std::swap(left, right);
        outgoing[static_cast<std::size_t>(left)].push_back(right);
    }

    std::vector<int> marked(static_cast<std::size_t>(vertex_count), -1);
    long long triangle_count = 0;
    for(int first = 0; first < vertex_count; ++first){
        for(int neighbor: outgoing[static_cast<std::size_t>(first)]){
            marked[static_cast<std::size_t>(neighbor)] = first;
        }
        for(int second: outgoing[static_cast<std::size_t>(first)]){
            for(int third: outgoing[static_cast<std::size_t>(second)]){
                if(marked[static_cast<std::size_t>(third)] != first) continue;
                std::array<int, 3> triangle{first, second, third};
                std::sort(triangle.begin(), triangle.end());
                callback(triangle[0], triangle[1], triangle[2]);
                ++triangle_count;
            }
        }
    }
    return triangle_count;
}

inline long long count_triangles(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return enumerate_triangles(vertex_count, edges, [](int, int, int){});
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ENUMERATE_TRIANGLES_HPP_INCLUDED
