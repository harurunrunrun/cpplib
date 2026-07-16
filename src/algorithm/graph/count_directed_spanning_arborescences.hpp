#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace count_directed_spanning_arborescences_internal{

inline __int128_t exact_determinant(
    std::vector<std::vector<__int128_t>> matrix
){
    const int size = static_cast<int>(matrix.size());
    if(size == 0) return 1;

    __int128_t previous_pivot = 1;
    int sign = 1;
    for(int column = 0; column + 1 < size; column++){
        int pivot = column;
        while(pivot < size &&
              matrix[static_cast<std::size_t>(pivot)]
                    [static_cast<std::size_t>(column)] == 0){
            pivot++;
        }
        if(pivot == size) return 0;
        if(pivot != column){
            std::swap(
                matrix[static_cast<std::size_t>(pivot)],
                matrix[static_cast<std::size_t>(column)]
            );
            sign = -sign;
        }

        const __int128_t current_pivot =
            matrix[static_cast<std::size_t>(column)]
                  [static_cast<std::size_t>(column)];
        for(int row = column + 1; row < size; row++){
            for(int next = column + 1; next < size; next++){
                const __int128_t numerator =
                    matrix[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(next)] * current_pivot -
                    matrix[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(column)] *
                    matrix[static_cast<std::size_t>(column)]
                          [static_cast<std::size_t>(next)];
                matrix[static_cast<std::size_t>(row)]
                      [static_cast<std::size_t>(next)] =
                    numerator / previous_pivot;
            }
            matrix[static_cast<std::size_t>(row)]
                  [static_cast<std::size_t>(column)] = 0;
        }
        previous_pivot = current_pivot;
    }
    return sign * matrix.back().back();
}

} // namespace count_directed_spanning_arborescences_internal

inline long long count_directed_spanning_arborescences(
    const std::vector<std::string>& adjacency
){
    const int vertex_count = static_cast<int>(adjacency.size());
    if(vertex_count <= 0 || 8 < vertex_count)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(count_directed_spanning_arborescences)."
        );
    }
    for(const auto& row: adjacency){
        if(static_cast<int>(row.size()) != vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-square matrix "
                "(count_directed_spanning_arborescences)."
            );
        }
        for(const char edge: row){
            if(edge != '0' && edge != '1')[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: non-binary matrix "
                    "(count_directed_spanning_arborescences)."
                );
            }
        }
    }

    std::vector<std::vector<__int128_t>> laplacian(
        static_cast<std::size_t>(vertex_count),
        std::vector<__int128_t>(static_cast<std::size_t>(vertex_count), 0)
    );
    for(int from = 0; from < vertex_count; from++){
        for(int to = 0; to < vertex_count; to++){
            if(from == to ||
               adjacency[static_cast<std::size_t>(from)]
                        [static_cast<std::size_t>(to)] == '0'){
                continue;
            }
            laplacian[static_cast<std::size_t>(to)]
                     [static_cast<std::size_t>(to)]++;
            laplacian[static_cast<std::size_t>(to)]
                     [static_cast<std::size_t>(from)]--;
        }
    }

    // L * 1 = 0.  The adjugate of L therefore has identical rows, and its
    // diagonal entries are the directed Matrix-Tree cofactors.  Consequently
    //
    //   det(L + 1 1^T) = 1^T adj(L) 1
    //                    = n * sum_root cofactor(root).
    //
    // This evaluates the sum for every root with one determinant instead of
    // evaluating n different minors.
    for(auto& row: laplacian){
        for(auto& value: row) value++;
    }
    const __int128_t determinant =
        count_directed_spanning_arborescences_internal::exact_determinant(
            std::move(laplacian)
        );
    const __int128_t answer = determinant / vertex_count;
    if(answer < 0 || answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "count_directed_spanning_arborescences overflow"
        );
    }
    return static_cast<long long>(answer);
}
