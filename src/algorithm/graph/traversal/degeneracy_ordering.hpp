#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct DegeneracyOrderingResult{
    std::vector<int> order;
    int degeneracy;
};

inline DegeneracyOrderingResult degeneracy_ordering(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(degeneracy_ordering)."
        );
    }
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto [from, to]: edges){
        if(from < 0 || from >= vertex_count
            || to < 0 || to >= vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: vertex out of range "
                "(degeneracy_ordering)."
            );
        }
        if(from == to) continue;
        graph[static_cast<std::size_t>(from)].push_back(to);
        graph[static_cast<std::size_t>(to)].push_back(from);
    }

    std::vector<int> degree(static_cast<std::size_t>(vertex_count));
    int maximum_degree = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        degree[static_cast<std::size_t>(vertex)] = static_cast<int>(
            graph[static_cast<std::size_t>(vertex)].size()
        );
        maximum_degree = std::max(
            maximum_degree, degree[static_cast<std::size_t>(vertex)]
        );
    }

    std::vector<int> head(static_cast<std::size_t>(maximum_degree + 1), -1);
    std::vector<int> previous(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> next(static_cast<std::size_t>(vertex_count), -1);
    const auto link = [&](int vertex, int value){
        const int old_head = head[static_cast<std::size_t>(value)];
        previous[static_cast<std::size_t>(vertex)] = -1;
        next[static_cast<std::size_t>(vertex)] = old_head;
        if(old_head != -1){
            previous[static_cast<std::size_t>(old_head)] = vertex;
        }
        head[static_cast<std::size_t>(value)] = vertex;
    };
    const auto unlink = [&](int vertex, int value){
        const int left = previous[static_cast<std::size_t>(vertex)];
        const int right = next[static_cast<std::size_t>(vertex)];
        if(left == -1){
            head[static_cast<std::size_t>(value)] = right;
        }else{
            next[static_cast<std::size_t>(left)] = right;
        }
        if(right != -1){
            previous[static_cast<std::size_t>(right)] = left;
        }
    };
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        link(vertex, degree[static_cast<std::size_t>(vertex)]);
    }

    DegeneracyOrderingResult result;
    result.order.reserve(static_cast<std::size_t>(vertex_count));
    result.degeneracy = 0;
    std::vector<unsigned char> removed(
        static_cast<std::size_t>(vertex_count), 0
    );
    int minimum_degree = 0;
    for(int removed_count = 0; removed_count < vertex_count; ++removed_count){
        while(head[static_cast<std::size_t>(minimum_degree)] == -1){
            ++minimum_degree;
        }
        const int vertex =
            head[static_cast<std::size_t>(minimum_degree)];
        unlink(vertex, minimum_degree);
        removed[static_cast<std::size_t>(vertex)] = 1;
        result.order.push_back(vertex);
        result.degeneracy = std::max(result.degeneracy, minimum_degree);
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(removed[static_cast<std::size_t>(to)] != 0) continue;
            const int old_degree = degree[static_cast<std::size_t>(to)];
            unlink(to, old_degree);
            const int new_degree = --degree[static_cast<std::size_t>(to)];
            link(to, new_degree);
            minimum_degree = std::min(minimum_degree, new_degree);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED
