#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_INSPHERE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_INSPHERE_HPP_INCLUDED

#include <array>
#include <stdexcept>

#include "../../../math/integer/exact_integer.hpp"

#include "exact_orient3d.hpp"

namespace geometry3d_integer_predicate_detail{

inline ExactInteger determinant3_exact(
    const std::array<std::array<ExactInteger, 3>, 3>& matrix
){
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
        - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
        + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

inline ExactInteger determinant4_exact(
    const std::array<std::array<ExactInteger, 4>, 4>& matrix
){
    ExactInteger result = 0;
    for(std::size_t column = 0; column < 4; ++column){
        std::array<std::array<ExactInteger, 3>, 3> minor{};
        for(std::size_t row = 1; row < 4; ++row){
            std::size_t destination = 0;
            for(std::size_t source = 0; source < 4; ++source){
                if(source == column) continue;
                minor[row - 1][destination++] = matrix[row][source];
            }
        }
        const ExactInteger term = matrix[0][column] * determinant3_exact(minor);
        result += column % 2 == 0 ? term : -term;
    }
    return result;
}

}  // namespace geometry3d_integer_predicate_detail

inline int exact_insphere(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third,
    const IntegerPoint3& fourth,
    const IntegerPoint3& query
){
    const int orientation = exact_orient3d(first, second, third, fourth);
    if(orientation == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate integer tetrahedron in exact_insphere");
    }
    const std::array<IntegerPoint3, 4> points{first, second, third, fourth};
    std::array<std::array<ExactInteger, 4>, 4> matrix{};
    for(std::size_t row = 0; row < points.size(); ++row){
        ExactInteger squared_norm = 0;
        for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
            const ExactInteger difference =
                ExactInteger(points[row][coordinate]) - query[coordinate];
            matrix[row][coordinate] = difference;
            squared_norm += difference * difference;
        }
        matrix[row][3] = squared_norm;
    }
    const int raw_sign = geometry3d_integer_predicate_detail::sign(
        geometry3d_integer_predicate_detail::determinant4_exact(matrix)
    );
    return orientation > 0 ? -raw_sign : raw_sign;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_INSPHERE_HPP_INCLUDED
