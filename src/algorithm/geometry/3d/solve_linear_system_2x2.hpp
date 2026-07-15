#pragma once

#include <array>
#include <optional>
#include <vector>

#include "gaussian_elimination_real.hpp"

inline std::optional<std::array<long double, 2>> solve_linear_system_2x2(
    const std::array<std::array<long double, 2>, 2>& matrix,
    const std::array<long double, 2>& right_hand_side,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    const auto solution = solve_square_linear_system_real(
        {{matrix[0][0], matrix[0][1]}, {matrix[1][0], matrix[1][1]}},
        {right_hand_side[0], right_hand_side[1]},
        relative_epsilon
    );
    if(!solution) return std::nullopt;
    return std::array<long double, 2>{(*solution)[0], (*solution)[1]};
}
