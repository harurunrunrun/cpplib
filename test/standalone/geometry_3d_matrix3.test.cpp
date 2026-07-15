// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>

#include "../../src/algorithm/geometry/3d/matrix3.hpp"

namespace{

bool close_value(
    long double left,
    long double right,
    long double epsilon = 1e-9L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

bool close_point(const Point3& left, const Point3& right, long double epsilon = 1e-9L){
    return close_value(left.x, right.x, epsilon) &&
        close_value(left.y, right.y, epsilon) &&
        close_value(left.z, right.z, epsilon);
}

bool close_matrix(const Matrix3& left, const Matrix3& right){
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = 0; column < 3; ++column){
            if(!close_value(
                left[row][column], right[row][column], 2e-9L
            )) return false;
        }
    }
    return true;
}

Matrix3 sample_matrix(){
    return Matrix3(Matrix3::Storage{{
        Matrix3::Row{{1, 2, 3}},
        Matrix3::Row{{0, 1, 4}},
        Matrix3::Row{{5, 6, 0}},
    }});
}

} // namespace

int main(){
    std::uint64_t seed = 0;
    std::size_t rounds = 0;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const Matrix3 identity = Matrix3::identity();
    const Matrix3 matrix = sample_matrix();
    if(!close_value(matrix.determinant(), 1)) return 1;
    if(!close_matrix(+matrix, matrix)) return 1;
    if(!close_matrix(matrix + (-matrix), Matrix3{})) return 1;
    if(!close_matrix(matrix - matrix, Matrix3{})) return 1;
    if(!close_matrix(matrix * identity, matrix)) return 1;
    if(!close_matrix(identity * matrix, matrix)) return 1;
    if(!close_matrix(matrix.transposed().transposed(), matrix)) return 1;

    const Point3 point{2, -1, 3};
    if(!close_point(matrix * point, {9, 11, 4})) return 1;
    const std::optional<Matrix3> inverted = matrix.inverse();
    if(!inverted || !close_matrix(matrix * *inverted, identity)) return 1;
    const std::optional<Point3> solution = matrix.solve(matrix * point);
    if(!solution || !close_point(*solution, point)) return 1;

    const Matrix3 singular(Matrix3::Storage{{
        Matrix3::Row{{1, 2, 3}},
        Matrix3::Row{{2, 4, 6}},
        Matrix3::Row{{0, 1, 0}},
    }});
    if(singular.inverse() || singular.solve({1, 2, 3})) return 1;

    std::mt19937_64 random(seed);
    std::uniform_real_distribution<long double> distribution(-4, 4);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        Matrix3 current;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                current[row][column] = distribution(random);
            }
            current[row][row] += 8;
        }
        const Point3 right{
            distribution(random), distribution(random), distribution(random),
        };
        const std::optional<Matrix3> current_inverse = current.inverse();
        if(!current_inverse) return 1;
        if(!close_matrix(current * *current_inverse, identity)) return 1;
        if(!close_matrix(*current_inverse * current, identity)) return 1;
        const std::optional<Point3> current_solution = current.solve(right);
        if(!current_solution) return 1;
        if(!close_point(current * *current_solution, right, 2e-8L)){
            return 1;
        }
        if(!close_point(
            current.transposed() * right,
            {
                current[0][0] * right.x + current[1][0] * right.y +
                    current[2][0] * right.z,
                current[0][1] * right.x + current[1][1] * right.y +
                    current[2][1] * right.z,
                current[0][2] * right.x + current[1][2] * right.y +
                    current[2][2] * right.z,
            }
        )) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
