#pragma once

#include <vector>

#include "tree_isomorphism.hpp"

inline bool tree_isomorphism_answer(
    const std::vector<std::vector<int>>& first,
    const std::vector<std::vector<int>>& second
){
    return tree_isomorphic(first, second);
}
