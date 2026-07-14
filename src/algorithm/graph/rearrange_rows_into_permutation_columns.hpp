#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "bipartite_edge_coloring.hpp"

inline std::vector<std::vector<int>> rearrange_rows_into_permutation_columns(
    const std::vector<std::vector<int>>& matrix
){
    if(matrix.empty()) return {};
    if(matrix.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::runtime_error(
            "library assertion fault: matrix is too large "
            "(rearrange_rows_into_permutation_columns)."
        );
    }

    const int row_count = static_cast<int>(matrix.size());
    const std::size_t column_count = matrix.front().size();
    if(column_count > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       (column_count != 0 && matrix.size() >
        static_cast<std::size_t>(std::numeric_limits<int>::max()) /
            column_count)){
        throw std::runtime_error(
            "library assertion fault: matrix is too large "
            "(rearrange_rows_into_permutation_columns)."
        );
    }

    std::vector<int> occurrences(matrix.size(), 0);
    std::vector<BipartiteEdgeColoringEdge> edges;
    edges.reserve(matrix.size() * column_count);
    for(int row = 0; row < row_count; ++row){
        if(matrix[static_cast<std::size_t>(row)].size() != column_count){
            throw std::runtime_error(
                "library assertion fault: jagged matrix "
                "(rearrange_rows_into_permutation_columns)."
            );
        }
        for(int value: matrix[static_cast<std::size_t>(row)]){
            if(value < 0 || row_count <= value){
                throw std::runtime_error(
                    "library assertion fault: range violation "
                    "(rearrange_rows_into_permutation_columns)."
                );
            }
            occurrences[static_cast<std::size_t>(value)]++;
            edges.push_back({row, value});
        }
    }
    for(int count: occurrences){
        if(static_cast<std::size_t>(count) != column_count){
            throw std::runtime_error(
                "library assertion fault: value multiplicity mismatch "
                "(rearrange_rows_into_permutation_columns)."
            );
        }
    }

    const auto coloring = bipartite_edge_coloring(
        row_count, row_count, edges
    );
    if(static_cast<std::size_t>(coloring.color_count) != column_count){
        throw std::logic_error(
            "library internal fault: unexpected edge color count "
            "(rearrange_rows_into_permutation_columns)."
        );
    }

    std::vector<std::vector<int>> result(
        matrix.size(), std::vector<int>(column_count)
    );
    std::size_t edge_id = 0;
    for(int row = 0; row < row_count; ++row){
        for(int value: matrix[static_cast<std::size_t>(row)]){
            result[static_cast<std::size_t>(row)][static_cast<std::size_t>(
                coloring.color[edge_id]
            )] = value;
            edge_id++;
        }
    }
    return result;
}
