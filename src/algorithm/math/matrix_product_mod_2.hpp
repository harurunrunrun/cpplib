#pragma once

#include <bitset>
#include <cstddef>
#include <stdexcept>
#include <vector>

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
