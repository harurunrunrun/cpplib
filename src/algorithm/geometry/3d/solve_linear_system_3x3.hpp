#pragma once

#include <optional>
#include <vector>

#include "gaussian_elimination_real.hpp"
#include "matrix3.hpp"

inline std::optional<Point3> solve_linear_system_3x3(
    const Matrix3& matrix,
    const Point3& right_hand_side,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    const auto solution = solve_square_linear_system_real(
        {
            {matrix[0][0], matrix[0][1], matrix[0][2]},
            {matrix[1][0], matrix[1][1], matrix[1][2]},
            {matrix[2][0], matrix[2][1], matrix[2][2]},
        },
        {right_hand_side.x, right_hand_side.y, right_hand_side.z},
        relative_epsilon
    );
    if(!solution) return std::nullopt;
    return Point3{(*solution)[0], (*solution)[1], (*solution)[2]};
}
