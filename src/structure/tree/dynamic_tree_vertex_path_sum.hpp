#ifndef CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_VERTEX_PATH_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_VERTEX_PATH_SUM_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../algorithm/tree/heavy_light_decomposition.hpp"

template<class T>
class DynamicTreeVertexPathSum{
    int _n;
    HeavyLightDecomposition decomposition;
    std::vector<T> fenwick;

    static int checked_size(std::size_t size){
        if(size > static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return static_cast<int>(size);
    }

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
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
    DynamicTreeVertexPathSum(
        const std::vector<T>& values,
        const std::vector<std::pair<int, int>>& edges,
        int root = 0
    ):
        _n(checked_size(values.size())),
        decomposition(_n),
        fenwick(static_cast<std::size_t>(_n) + 1, T{}){
        const std::size_t expected_edges = _n == 0
            ? 0
            : static_cast<std::size_t>(_n - 1);
        if(edges.size() != expected_edges || (_n == 0 && root != 0)){
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (constructor)."
            );
        }
        for(const auto [u, v]: edges){
            decomposition.add_edge(u, v);
        }
        decomposition.build(root);

        std::vector<T> ordered(static_cast<std::size_t>(_n), T{});
        for(int vertex = 0; vertex < _n; ++vertex){
            ordered[static_cast<std::size_t>(decomposition.in(vertex))] =
                values[static_cast<std::size_t>(vertex)];
        }
        for(int index = 1; index <= _n; ++index){
            fenwick[static_cast<std::size_t>(index)] +=
                ordered[static_cast<std::size_t>(index - 1)];
            const int parent = index + (index & -index);
            if(parent <= _n){
                fenwick[static_cast<std::size_t>(parent)] +=
                    fenwick[static_cast<std::size_t>(index)];
            }
        }
    }

    int size() const{
        return _n;
    }

    void add(int vertex, const T& delta){
        check_vertex(
            vertex,
            "library assertion fault: range violation (add)."
        );
        add_index(decomposition.in(vertex), delta);
    }

    T path_sum(int u, int v) const{
        check_vertex(
            u,
            "library assertion fault: range violation (path_sum)."
        );
        check_vertex(
            v,
            "library assertion fault: range violation (path_sum)."
        );
        T result{};
        for(const auto [left, right]: decomposition.path_query(u, v)){
            result += range_sum(left, right);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_TREE_VERTEX_PATH_SUM_HPP_INCLUDED
