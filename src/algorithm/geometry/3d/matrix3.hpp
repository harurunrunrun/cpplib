#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>

#include "base.hpp"

struct Matrix3{
    using Row = std::array<long double, 3>;
    using Storage = std::array<Row, 3>;

    Storage values{};

    Matrix3() = default;
    explicit constexpr Matrix3(const Storage& values_): values(values_){}

    constexpr Row& operator[](std::size_t row){ return values[row]; }
    constexpr const Row& operator[](std::size_t row) const{ return values[row]; }

    static constexpr Matrix3 identity(){
        return Matrix3(Storage{{
            Row{{1, 0, 0}},
            Row{{0, 1, 0}},
            Row{{0, 0, 1}},
        }});
    }

    constexpr Matrix3 operator+() const{ return *this; }

    constexpr Matrix3 operator-() const{
        Matrix3 result;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                result[row][column] = -values[row][column];
            }
        }
        return result;
    }

    constexpr Matrix3& operator+=(const Matrix3& other){
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                values[row][column] += other[row][column];
            }
        }
        return *this;
    }

    constexpr Matrix3& operator-=(const Matrix3& other){
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                values[row][column] -= other[row][column];
            }
        }
        return *this;
    }

    constexpr Matrix3& operator*=(const Matrix3& other){
        *this = *this * other;
        return *this;
    }

    constexpr Matrix3 transposed() const{
        Matrix3 result;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                result[row][column] = values[column][row];
            }
        }
        return result;
    }

    constexpr long double determinant() const{
        return
            values[0][0] * (
                values[1][1] * values[2][2] -
                values[1][2] * values[2][1]
            ) -
            values[0][1] * (
                values[1][0] * values[2][2] -
                values[1][2] * values[2][0]
            ) +
            values[0][2] * (
                values[1][0] * values[2][1] -
                values[1][1] * values[2][0]
            );
    }

    std::optional<Matrix3> inverse(
        long double relative_epsilon = GEOMETRY3D_EPS
    ) const{
        if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
            return std::nullopt;
        }
        long double scale = 0;
        for(const Row& row: values){
            for(long double value: row){
                if(!std::isfinite(value)) return std::nullopt;
                scale = std::max(scale, std::abs(value));
            }
        }
        if(scale == 0) return std::nullopt;
        Matrix3 normalized;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                normalized[row][column] = values[row][column] / scale;
            }
        }
        const long double det = normalized.determinant();
        if(!std::isfinite(det) || std::abs(det) <= relative_epsilon){
            return std::nullopt;
        }

        Matrix3 adjugate;
        adjugate[0][0] = normalized[1][1] * normalized[2][2]
            - normalized[1][2] * normalized[2][1];
        adjugate[0][1] = normalized[0][2] * normalized[2][1]
            - normalized[0][1] * normalized[2][2];
        adjugate[0][2] = normalized[0][1] * normalized[1][2]
            - normalized[0][2] * normalized[1][1];
        adjugate[1][0] = normalized[1][2] * normalized[2][0]
            - normalized[1][0] * normalized[2][2];
        adjugate[1][1] = normalized[0][0] * normalized[2][2]
            - normalized[0][2] * normalized[2][0];
        adjugate[1][2] = normalized[0][2] * normalized[1][0]
            - normalized[0][0] * normalized[1][2];
        adjugate[2][0] = normalized[1][0] * normalized[2][1]
            - normalized[1][1] * normalized[2][0];
        adjugate[2][1] = normalized[0][1] * normalized[2][0]
            - normalized[0][0] * normalized[2][1];
        adjugate[2][2] = normalized[0][0] * normalized[1][1]
            - normalized[0][1] * normalized[1][0];

        for(Row& row: adjugate.values){
            for(long double& value: row){
                value /= det;
                value /= scale;
                if(!std::isfinite(value)) return std::nullopt;
            }
        }
        return adjugate;
    }

    std::optional<Point3> solve(
        const Point3& right_hand_side,
        long double relative_epsilon = GEOMETRY3D_EPS
    ) const{
        const std::optional<Matrix3> inverted = inverse(relative_epsilon);
        if(!inverted) return std::nullopt;
        return *inverted * right_hand_side;
    }

    friend constexpr Matrix3 operator+(Matrix3 left, const Matrix3& right){
        left += right;
        return left;
    }

    friend constexpr Matrix3 operator-(Matrix3 left, const Matrix3& right){
        left -= right;
        return left;
    }

    friend constexpr Matrix3 operator*(const Matrix3& left, const Matrix3& right){
        Matrix3 result;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t middle = 0; middle < 3; ++middle){
                for(std::size_t column = 0; column < 3; ++column){
                    result[row][column] +=
                        left[row][middle] * right[middle][column];
                }
            }
        }
        return result;
    }

    friend Point3 operator*(const Matrix3& matrix, const Point3& point){
        return {
            matrix[0][0] * point.x + matrix[0][1] * point.y +
                matrix[0][2] * point.z,
            matrix[1][0] * point.x + matrix[1][1] * point.y +
                matrix[1][2] * point.z,
            matrix[2][0] * point.x + matrix[2][1] * point.y +
                matrix[2][2] * point.z,
        };
    }
};
