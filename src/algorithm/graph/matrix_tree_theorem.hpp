#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct MatrixTreeEdge{
    int from;
    int to;
    T weight;
};

template<class T, std::size_t MAX_SIZE>
using MatrixTreeFixedMatrix =
    std::array<std::array<T, MAX_SIZE>, MAX_SIZE>;

namespace matrix_tree_theorem_internal{

template<class T>
void validate_edges(
    int n,
    const std::vector<MatrixTreeEdge<T>>& edges,
    const char* message
){
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
}

template<std::size_t MAX_VERTEX>
void validate_vertex_count(int n, const char* message){
    if(n < 0 || static_cast<std::size_t>(n) > MAX_VERTEX)[[unlikely]]{
        throw std::runtime_error(message);
    }
}

int minor_index(int vertex, int removed){
    return vertex < removed ? vertex : vertex - 1;
}

} // namespace matrix_tree_theorem_internal

template<class T, std::size_t MAX_SIZE>
T matrix_tree_determinant(
    MatrixTreeFixedMatrix<T, MAX_SIZE> matrix,
    int n
){
    if(n < 0 || static_cast<std::size_t>(n) > MAX_SIZE)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (matrix_tree_determinant)."
        );
    }

    T determinant = T(1);
    for(int column = 0; column < n; column++){
        int pivot = column;
        while(pivot < n &&
            matrix[static_cast<std::size_t>(pivot)][static_cast<std::size_t>(column)]
                == T()
        ){
            pivot++;
        }
        if(pivot == n) return T();

        if(pivot != column){
            for(int j = column; j < n; j++){
                std::swap(
                    matrix[static_cast<std::size_t>(pivot)][static_cast<std::size_t>(j)],
                    matrix[static_cast<std::size_t>(column)][static_cast<std::size_t>(j)]
                );
            }
            determinant = -determinant;
        }

        const T pivot_value =
            matrix[static_cast<std::size_t>(column)][static_cast<std::size_t>(column)];
        determinant *= pivot_value;
        const T inverse = T(1) / pivot_value;
        for(int row = column + 1; row < n; row++){
            T& leading =
                matrix[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)];
            if(leading == T()) continue;
            const T factor = leading * inverse;
            leading = T();
            for(int j = column + 1; j < n; j++){
                matrix[static_cast<std::size_t>(row)][static_cast<std::size_t>(j)]
                    -= factor *
                    matrix[static_cast<std::size_t>(column)][static_cast<std::size_t>(j)];
            }
        }
    }
    return determinant;
}

template<class T, std::size_t MAX_VERTEX>
T count_weighted_undirected_spanning_trees(
    int n,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(count_weighted_undirected_spanning_trees).";
    matrix_tree_theorem_internal::validate_vertex_count<MAX_VERTEX>(n, message);
    matrix_tree_theorem_internal::validate_edges(n, edges, message);
    if(n == 0) return T(1);

    constexpr int removed = 0;
    MatrixTreeFixedMatrix<T, MAX_VERTEX> minor{};
    for(const auto& edge: edges){
        if(edge.from == edge.to) continue;
        const int from = matrix_tree_theorem_internal::minor_index(
            edge.from, removed
        );
        const int to = matrix_tree_theorem_internal::minor_index(
            edge.to, removed
        );
        if(edge.from != removed){
            minor[static_cast<std::size_t>(from)][static_cast<std::size_t>(from)]
                += edge.weight;
        }
        if(edge.to != removed){
            minor[static_cast<std::size_t>(to)][static_cast<std::size_t>(to)]
                += edge.weight;
        }
        if(edge.from != removed && edge.to != removed){
            minor[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)]
                -= edge.weight;
            minor[static_cast<std::size_t>(to)][static_cast<std::size_t>(from)]
                -= edge.weight;
        }
    }
    return matrix_tree_determinant<T, MAX_VERTEX>(std::move(minor), n - 1);
}

template<class T, std::size_t MAX_VERTEX>
T count_weighted_directed_out_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(count_weighted_directed_out_arborescences).";
    matrix_tree_theorem_internal::validate_vertex_count<MAX_VERTEX>(n, message);
    if(root < 0 || n <= root)[[unlikely]] throw std::runtime_error(message);
    matrix_tree_theorem_internal::validate_edges(n, edges, message);

    MatrixTreeFixedMatrix<T, MAX_VERTEX> minor{};
    for(const auto& edge: edges){
        if(edge.from == edge.to || edge.to == root) continue;
        const int to = matrix_tree_theorem_internal::minor_index(edge.to, root);
        minor[static_cast<std::size_t>(to)][static_cast<std::size_t>(to)]
            += edge.weight;
        if(edge.from != root){
            const int from = matrix_tree_theorem_internal::minor_index(
                edge.from, root
            );
            minor[static_cast<std::size_t>(to)][static_cast<std::size_t>(from)]
                -= edge.weight;
        }
    }
    return matrix_tree_determinant<T, MAX_VERTEX>(std::move(minor), n - 1);
}

template<class T, std::size_t MAX_VERTEX>
T count_weighted_directed_in_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(count_weighted_directed_in_arborescences).";
    matrix_tree_theorem_internal::validate_vertex_count<MAX_VERTEX>(n, message);
    if(root < 0 || n <= root)[[unlikely]] throw std::runtime_error(message);
    matrix_tree_theorem_internal::validate_edges(n, edges, message);

    MatrixTreeFixedMatrix<T, MAX_VERTEX> minor{};
    for(const auto& edge: edges){
        if(edge.from == edge.to || edge.from == root) continue;
        const int from = matrix_tree_theorem_internal::minor_index(
            edge.from, root
        );
        minor[static_cast<std::size_t>(from)][static_cast<std::size_t>(from)]
            += edge.weight;
        if(edge.to != root){
            const int to = matrix_tree_theorem_internal::minor_index(
                edge.to, root
            );
            minor[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)]
                -= edge.weight;
        }
    }
    return matrix_tree_determinant<T, MAX_VERTEX>(std::move(minor), n - 1);
}
