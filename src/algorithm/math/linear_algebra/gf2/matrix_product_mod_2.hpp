#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_MATRIX_PRODUCT_MOD_2_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_MATRIX_PRODUCT_MOD_2_HPP_INCLUDED

#include <bitset>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "../elimination/gaussian_elimination.hpp"

template<std::size_t MAX_INNER, std::size_t MAX_COLUMNS>
std::vector<std::bitset<MAX_COLUMNS>> matrix_product_mod_2(
    const std::vector<std::bitset<MAX_INNER>>& lhs,
    const std::vector<std::bitset<MAX_COLUMNS>>& rhs,
    std::size_t inner_size,
    std::size_t result_columns
){
    if(inner_size > MAX_INNER || result_columns > MAX_COLUMNS ||
       rhs.size() != inner_size)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: shape violation (matrix_product_mod_2)."
        );
    }

    std::bitset<MAX_COLUMNS> active_columns;
    if(result_columns == MAX_COLUMNS){
        active_columns.set();
    }else{
        for(std::size_t column = 0; column < result_columns; ++column){
            active_columns.set(column);
        }
    }

    std::vector<std::bitset<MAX_COLUMNS>> result;
    result.reserve(lhs.size());
    for(const auto& lhs_row: lhs){
        std::bitset<MAX_COLUMNS> result_row;
        for(std::size_t column = 0; column < inner_size; ++column){
            if(lhs_row.test(column)) result_row ^= rhs[column];
        }
        result_row &= active_columns;
        result.push_back(result_row);
    }
    return result;
}

inline std::vector<std::vector<bool>> matrix_product_mod_2(
    const std::vector<std::vector<bool>>& lhs, const std::vector<std::vector<bool>>& rhs,
    std::size_t inner_size, std::size_t result_columns
){
    if(rhs.size() != inner_size) throw std::invalid_argument("GF(2) product shape");
    const auto right = gf2_dynamic_internal::pack(rhs, result_columns);
    std::vector<std::vector<bool>> result(lhs.size(), std::vector<bool>(result_columns));
    std::vector<std::uint64_t> row((result_columns + 63) / 64);
    for(std::size_t i = 0; i < lhs.size(); ++i){
        if(lhs[i].size() < inner_size) throw std::invalid_argument("GF(2) product shape");
        std::fill(row.begin(), row.end(), 0);
        for(std::size_t j = 0; j < inner_size; ++j) if(lhs[i][j])
            for(std::size_t word = 0; word < row.size(); ++word) row[word] ^= right[j][word];
        for(std::size_t j = 0; j < result_columns; ++j) result[i][j] = gf2_dynamic_internal::bit(row, j);
    }
    return result;
}
inline std::vector<std::vector<bool>> matrix_product_mod_2(
    const std::vector<std::vector<bool>>& lhs, const std::vector<std::vector<bool>>& rhs
){
    return matrix_product_mod_2(lhs, rhs, rhs.size(), rhs.empty() ? 0 : rhs.front().size());
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_MATRIX_PRODUCT_MOD_2_HPP_INCLUDED
