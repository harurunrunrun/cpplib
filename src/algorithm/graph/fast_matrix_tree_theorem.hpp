#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FAST_MATRIX_TREE_THEOREM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FAST_MATRIX_TREE_THEOREM_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "matrix_tree_theorem.hpp"
#include "../math/fast_determinant.hpp"

namespace fast_matrix_tree_theorem_internal{

template<class T>
void validate(int n, const std::vector<MatrixTreeEdge<T>>& edges){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(fast matrix-tree theorem)."
        );
    }
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to){
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(fast matrix-tree theorem)."
            );
        }
    }
}

inline std::size_t minor_index(int vertex, int removed){
    return static_cast<std::size_t>(vertex < removed ? vertex : vertex - 1);
}

}  // namespace fast_matrix_tree_theorem_internal

template<class T>
T fast_count_weighted_undirected_spanning_trees(
    int n,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    fast_matrix_tree_theorem_internal::validate(n, edges);
    if(n == 0) return T(1);
    const std::size_t size = static_cast<std::size_t>(n - 1);
    std::vector<T> minor(fast_determinant_internal::checked_square(size));
    constexpr int removed = 0;
    for(const auto& edge: edges){
        if(edge.from == edge.to) continue;
        if(edge.from != removed){
            const std::size_t from =
                fast_matrix_tree_theorem_internal::minor_index(
                    edge.from, removed
                );
            minor[from * size + from] += edge.weight;
        }
        if(edge.to != removed){
            const std::size_t to =
                fast_matrix_tree_theorem_internal::minor_index(
                    edge.to, removed
                );
            minor[to * size + to] += edge.weight;
        }
        if(edge.from != removed && edge.to != removed){
            const std::size_t from =
                fast_matrix_tree_theorem_internal::minor_index(
                    edge.from, removed
                );
            const std::size_t to =
                fast_matrix_tree_theorem_internal::minor_index(
                    edge.to, removed
                );
            minor[from * size + to] -= edge.weight;
            minor[to * size + from] -= edge.weight;
        }
    }
    return fast_determinant(minor, size);
}

template<class T>
T fast_count_weighted_directed_out_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    fast_matrix_tree_theorem_internal::validate(n, edges);
    if(root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(fast matrix-tree theorem)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(n - 1);
    std::vector<T> minor(fast_determinant_internal::checked_square(size));
    for(const auto& edge: edges){
        if(edge.from == edge.to || edge.to == root) continue;
        const std::size_t to =
            fast_matrix_tree_theorem_internal::minor_index(edge.to, root);
        minor[to * size + to] += edge.weight;
        if(edge.from != root){
            const std::size_t from =
                fast_matrix_tree_theorem_internal::minor_index(edge.from, root);
            minor[to * size + from] -= edge.weight;
        }
    }
    return fast_determinant(minor, size);
}

template<class T>
T fast_count_weighted_directed_in_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    fast_matrix_tree_theorem_internal::validate(n, edges);
    if(root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(fast matrix-tree theorem)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(n - 1);
    std::vector<T> minor(fast_determinant_internal::checked_square(size));
    for(const auto& edge: edges){
        if(edge.from == edge.to || edge.from == root) continue;
        const std::size_t from =
            fast_matrix_tree_theorem_internal::minor_index(edge.from, root);
        minor[from * size + from] += edge.weight;
        if(edge.to != root){
            const std::size_t to =
                fast_matrix_tree_theorem_internal::minor_index(edge.to, root);
            minor[from * size + to] -= edge.weight;
        }
    }
    return fast_determinant(minor, size);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FAST_MATRIX_TREE_THEOREM_HPP_INCLUDED
