#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED

#include <optional>
#include <vector>

#include "dancing_links.hpp"

inline std::optional<std::vector<int>> exact_cover(
    int column_count,
    const std::vector<std::vector<int>>& rows
){
    DancingLinks solver(column_count);
    for(const auto& row: rows) solver.add_row(row);
    auto solutions = solver.solve(1);
    if(solutions.empty()) return std::nullopt;
    return solutions.front();
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED
