#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_ELLIPSE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_ELLIPSE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../type/definition/minimum_enclosing_ellipse_result.hpp"

namespace minimum_enclosing_ellipse_detail{

using Matrix3 = std::array<std::array<long double, 3>, 3>;

inline Matrix3 inverse(Matrix3 matrix){
    Matrix3 result{{
        {{1.0L, 0.0L, 0.0L}},
        {{0.0L, 1.0L, 0.0L}},
        {{0.0L, 0.0L, 1.0L}},
    }};
    for(std::size_t column = 0; column < 3; ++column){
        std::size_t pivot = column;
        for(std::size_t row = column + 1; row < 3; ++row){
            if(std::abs(matrix[row][column])
                > std::abs(matrix[pivot][column])){
                pivot = row;
            }
        }
        if(plus_geometry_detail::sign(
            matrix[pivot][column]
        ) == 0){
            throw std::domain_error(
                "minimum enclosing ellipse is degenerate"
            );
        }
        std::swap(matrix[pivot], matrix[column]);
        std::swap(result[pivot], result[column]);
        const long double divisor = matrix[column][column];
        for(std::size_t index = 0; index < 3; ++index){
            matrix[column][index] /= divisor;
            result[column][index] /= divisor;
        }
        for(std::size_t row = 0; row < 3; ++row){
            if(row == column) continue;
            const long double factor = matrix[row][column];
            for(std::size_t index = 0; index < 3; ++index){
                matrix[row][index] -= factor * matrix[column][index];
                result[row][index] -= factor * result[column][index];
            }
        }
    }
    return result;
}

inline long double leverage(const Point& point, const Matrix3& inverse_matrix){
    const std::array<long double, 3> vector{point.x, point.y, 1.0L};
    long double result = 0.0L;
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = 0; column < 3; ++column){
            result += vector[row] * inverse_matrix[row][column] * vector[column];
        }
    }
    return result;
}

}  // namespace minimum_enclosing_ellipse_detail

inline MinimumEnclosingEllipseResult minimum_enclosing_ellipse(
    const std::vector<Point>& points,
    long double tolerance = 1.0e-4L,
    std::size_t maximum_iterations = 200000
){
    if(points.size() < 3){
        throw std::invalid_argument(
            "a positive-area enclosing ellipse requires at least three points"
        );
    }
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance)){
        throw std::invalid_argument(
            "ellipse approximation tolerance must be positive and finite"
        );
    }
    for(const Point& point: points){
        plus_geometry_detail::validate_finite(point);
    }
    std::vector<long double> weights(
        points.size(), 1.0L / static_cast<long double>(points.size())
    );
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        minimum_enclosing_ellipse_detail::Matrix3 moment{};
        for(std::size_t index = 0; index < points.size(); ++index){
            const std::array<long double, 3> vector{
                points[index].x, points[index].y, 1.0L
            };
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = 0; column < 3; ++column){
                    moment[row][column] +=
                        weights[index] * vector[row] * vector[column];
                }
            }
        }
        const auto inverse =
            minimum_enclosing_ellipse_detail::inverse(moment);
        std::size_t maximum_index = 0;
        long double maximum_value =
            minimum_enclosing_ellipse_detail::leverage(
                points[0], inverse
            );
        for(std::size_t index = 1; index < points.size(); ++index){
            const long double value =
                minimum_enclosing_ellipse_detail::leverage(
                    points[index], inverse
                );
            if(value > maximum_value){
                maximum_value = value;
                maximum_index = index;
            }
        }
        if(maximum_value <= 3.0L + tolerance) break;
        const long double step = (maximum_value - 3.0L)
            / (3.0L * (maximum_value - 1.0L));
        for(long double& weight: weights) weight *= 1.0L - step;
        weights[maximum_index] += step;
        if(iteration + 1 == maximum_iterations){
            throw std::runtime_error(
                "minimum enclosing ellipse iteration limit was reached"
            );
        }
    }

    Point center{};
    for(std::size_t index = 0; index < points.size(); ++index){
        center += points[index] * weights[index];
    }
    long double xx = 0.0L;
    long double xy = 0.0L;
    long double yy = 0.0L;
    for(std::size_t index = 0; index < points.size(); ++index){
        const Point difference = points[index] - center;
        xx += weights[index] * difference.x * difference.x;
        xy += weights[index] * difference.x * difference.y;
        yy += weights[index] * difference.y * difference.y;
    }
    const long double determinant = xx * yy - xy * xy;
    if(plus_geometry_detail::sign(
        determinant,
        std::abs(xx * yy) + std::abs(xy * xy)
    ) <= 0){
        throw std::domain_error("minimum enclosing ellipse is degenerate");
    }
    MinimumEnclosingEllipseResult result;
    result.center = center;
    result.quadratic_form = {{
        {{yy / (2.0L * determinant), -xy / (2.0L * determinant)}},
        {{-xy / (2.0L * determinant), xx / (2.0L * determinant)}},
    }};
    long double maximum_quadratic_value = 0.0L;
    for(const Point& point: points){
        const Point difference = point - result.center;
        const long double value =
            difference.x * (
                result.quadratic_form[0][0] * difference.x
                + result.quadratic_form[0][1] * difference.y
            )
            + difference.y * (
                result.quadratic_form[1][0] * difference.x
                + result.quadratic_form[1][1] * difference.y
            );
        maximum_quadratic_value =
            std::max(maximum_quadratic_value, value);
    }
    if(maximum_quadratic_value > 1.0L){
        for(auto& row: result.quadratic_form){
            for(long double& value: row){
                value /= maximum_quadratic_value;
            }
        }
    }
    result.approximation_tolerance = tolerance;
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_ELLIPSE_HPP_INCLUDED
