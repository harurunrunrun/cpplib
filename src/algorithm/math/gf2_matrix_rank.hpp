#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline std::size_t gf2_matrix_rank(
    const std::vector<std::string>& matrix,
    std::size_t column_count
){
    if(matrix.empty()) return 0;
    const std::size_t word_count =
        column_count / 64 + static_cast<std::size_t>(column_count % 64 != 0);
    std::vector<std::vector<std::uint64_t>> rows(
        matrix.size(), std::vector<std::uint64_t>(word_count)
    );
    for(std::size_t row = 0; row < matrix.size(); ++row){
        if(matrix[row].size() != column_count)[[unlikely]]{
            throw std::invalid_argument(
                "gf2_matrix_rank requires rows of the declared width"
            );
        }
        for(std::size_t column = 0; column < column_count; ++column){
            const char value = matrix[row][column];
            if(value != '0' && value != '1')[[unlikely]]{
                throw std::invalid_argument(
                    "gf2_matrix_rank accepts only binary characters"
                );
            }
            if(value == '1'){
                rows[row][column >> 6] |=
                    std::uint64_t{1} << (column & 63);
            }
        }
    }

    std::size_t rank = 0;
    for(std::size_t column = 0;
        column < column_count && rank < rows.size(); ++column){
        std::size_t pivot = rank;
        while(pivot < rows.size() &&
              ((rows[pivot][column >> 6] >> (column & 63)) & 1U) == 0){
            ++pivot;
        }
        if(pivot == rows.size()) continue;
        if(pivot != rank) std::swap(rows[pivot], rows[rank]);
        for(std::size_t row = rank + 1; row < rows.size(); ++row){
            if(((rows[row][column >> 6] >> (column & 63)) & 1U) == 0){
                continue;
            }
            for(std::size_t word = column >> 6; word < word_count; ++word){
                rows[row][word] ^= rows[rank][word];
            }
        }
        ++rank;
    }
    return rank;
}

inline std::size_t gf2_matrix_rank(
    const std::vector<std::string>& matrix
){
    return gf2_matrix_rank(matrix, matrix.empty() ? 0 : matrix.front().size());
}
