#ifndef CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_EDGE_PATH_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_EDGE_PATH_SUM_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "../../algorithm/tree/heavy_light_decomposition.hpp"

template<class T>
class DynamicTreeEdgePathSum{
    int _n;
    HeavyLightDecomposition decomposition;
    std::vector<int> edge_position;
    std::vector<T> edge_values;
    std::vector<T> fenwick;

    static int checked_vertex_count(int vertex_count){
        if(vertex_count < 0){
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return vertex_count;
    }

    void check_edge(int edge) const{
        if(edge < 0 || static_cast<int>(edge_values.size()) <= edge)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set_edge)."
            );
        }
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (path_sum)."
            );
        }
    }

    void add_index(int index, const T& delta){
        for(++index; index <= _n; index += index & -index){
            fenwick[static_cast<std::size_t>(index)] += delta;
        }
    }

    T prefix_sum(int right) const{
        T result{};
        for(; right > 0; right -= right & -right){
            result += fenwick[static_cast<std::size_t>(right)];
        }
        return result;
    }

    T range_sum(int left, int right) const{
        return prefix_sum(right) - prefix_sum(left);
    }

public:
    DynamicTreeEdgePathSum(
        int vertex_count,
        const std::vector<std::tuple<int, int, T>>& edges,
        int root = 0
    ):
        _n(checked_vertex_count(vertex_count)),
        decomposition(_n),
        edge_position(edges.size()),
        edge_values(edges.size()),
        fenwick(static_cast<std::size_t>(_n) + 1, T{}){
        if(edges.size() != static_cast<std::size_t>(_n == 0 ? 0 : _n - 1)
            || (_n == 0 && root != 0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (constructor)."
            );
        }
        for(const auto& [u, v, weight]: edges){
            (void)weight;
            decomposition.add_edge(u, v);
        }
        decomposition.build(root);
        for(std::size_t edge = 0; edge < edges.size(); ++edge){
            const auto [u, v, weight] = edges[edge];
            const int child = decomposition.parent(u) == v ? u : v;
            if(decomposition.parent(child) != (child == u ? v : u))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (constructor)."
                );
            }
            edge_position[edge] = decomposition.in(child);
            edge_values[edge] = weight;
            add_index(edge_position[edge], weight);
        }
    }

    int size() const{
        return _n;
    }

    int edge_count() const{
        return static_cast<int>(edge_values.size());
    }

    const T& edge_value(int edge) const{
        check_edge(edge);
        return edge_values[static_cast<std::size_t>(edge)];
    }

    void set_edge(int edge, const T& value){
        check_edge(edge);
        const std::size_t index = static_cast<std::size_t>(edge);
        add_index(edge_position[index], value - edge_values[index]);
        edge_values[index] = value;
    }

    T path_sum(int u, int v) const{
        check_vertex(u);
        check_vertex(v);
        T result{};
        for(const auto [left, right]: decomposition.path_query(u, v, false)){
            result += range_sum(left, right);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_EDGE_PATH_SUM_HPP_INCLUDED
