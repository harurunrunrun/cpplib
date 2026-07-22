#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_AFFINE_TRANSFORM3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_AFFINE_TRANSFORM3_HPP_INCLUDED

#include <array>
#include <cmath>
#include <cstddef>
#include <optional>

#include "base.hpp"
#include "dot.hpp"
#include "line3_direction.hpp"
#include "matrix3.hpp"
#include "plane3_unit_normal.hpp"
#include "unit.hpp"

struct AffineTransform3{
    Matrix3 linear = Matrix3::identity();
    Point3 offset{};

    AffineTransform3() = default;
    AffineTransform3(const Matrix3& linear_, const Point3& offset_)
        : linear(linear_), offset(offset_){}

    static AffineTransform3 identity(){ return {}; }

    static AffineTransform3 translation(const Point3& displacement){
        return {Matrix3::identity(), displacement};
    }

    static AffineTransform3 uniform_scaling(long double factor){
        return nonuniform_scaling({factor, factor, factor});
    }

    static AffineTransform3 nonuniform_scaling(const Point3& factors){
        return {
            Matrix3(Matrix3::Storage{{
                Matrix3::Row{{factors.x, 0, 0}},
                Matrix3::Row{{0, factors.y, 0}},
                Matrix3::Row{{0, 0, factors.z}},
            }}),
            {},
        };
    }

    static AffineTransform3 rotation_about_origin(
        const Point3& axis,
        long double theta
    ){
        const Point3 direction = unit(axis);
        const long double x = direction.x;
        const long double y = direction.y;
        const long double z = direction.z;
        const long double cosine = std::cos(theta);
        const long double sine = std::sin(theta);
        const long double one_minus_cosine = 1 - cosine;
        return {
            Matrix3(Matrix3::Storage{{
                Matrix3::Row{{
                    cosine + x * x * one_minus_cosine,
                    x * y * one_minus_cosine - z * sine,
                    x * z * one_minus_cosine + y * sine,
                }},
                Matrix3::Row{{
                    y * x * one_minus_cosine + z * sine,
                    cosine + y * y * one_minus_cosine,
                    y * z * one_minus_cosine - x * sine,
                }},
                Matrix3::Row{{
                    z * x * one_minus_cosine - y * sine,
                    z * y * one_minus_cosine + x * sine,
                    cosine + z * z * one_minus_cosine,
                }},
            }}),
            {},
        };
    }

    static AffineTransform3 rotation_about_axis(
        const Line3& axis,
        long double theta
    ){
        const AffineTransform3 rotation = rotation_about_origin(
            line3_direction(axis), theta
        );
        return {
            rotation.linear,
            axis.a - rotation.linear * axis.a,
        };
    }

    static AffineTransform3 rotation_about_point(
        const Point3& center,
        const Point3& axis,
        long double theta
    ){
        const AffineTransform3 rotation = rotation_about_origin(axis, theta);
        return {
            rotation.linear,
            center - rotation.linear * center,
        };
    }

    static AffineTransform3 reflection_across_plane(const Plane3& plane){
        const Point3 normal = plane3_unit_normal(plane);
        Matrix3 result = Matrix3::identity();
        const std::array<long double, 3> coordinates{{
            normal.x, normal.y, normal.z,
        }};
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                result[row][column] -=
                    2 * coordinates[row] * coordinates[column];
            }
        }
        return {result, normal * (2 * dot(normal, plane.point))};
    }

    static AffineTransform3 half_turn_about_line(const Line3& line){
        const Point3 direction = unit(line3_direction(line));
        Matrix3 result = -Matrix3::identity();
        const std::array<long double, 3> coordinates{{
            direction.x, direction.y, direction.z,
        }};
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                result[row][column] +=
                    2 * coordinates[row] * coordinates[column];
            }
        }
        return {result, line.a - result * line.a};
    }

    Point3 apply(const Point3& point) const{
        return linear * point + offset;
    }

    Point3 operator()(const Point3& point) const{
        return apply(point);
    }

    AffineTransform3 compose(const AffineTransform3& inner) const{
        return {
            linear * inner.linear,
            linear * inner.offset + offset,
        };
    }

    std::optional<AffineTransform3> inverse(
        long double relative_epsilon = GEOMETRY3D_EPS
    ) const{
        const std::optional<Matrix3> inverse_linear =
            linear.inverse(relative_epsilon);
        if(!inverse_linear) return std::nullopt;
        return AffineTransform3{
            *inverse_linear,
            -(*inverse_linear * offset),
        };
    }

    friend AffineTransform3 operator*(
        const AffineTransform3& outer,
        const AffineTransform3& inner
    ){
        return outer.compose(inner);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_AFFINE_TRANSFORM3_HPP_INCLUDED
