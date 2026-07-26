// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
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

    for(const long double scale: {1e-2000L, 1e2000L}){
        const Matrix3 scaled(Matrix3::Storage{{
            Matrix3::Row{{2 * scale, 0, 0}},
            Matrix3::Row{{0, 3 * scale, 0}},
            Matrix3::Row{{0, 0, 4 * scale}},
        }});
        const std::optional<Matrix3> scaled_inverse = scaled.inverse();
        if(!scaled_inverse || !close_matrix(
            scaled * *scaled_inverse, identity
        ) || !close_value((*scaled_inverse)[0][0] * scale, 0.5L)
            || !close_value((*scaled_inverse)[1][1] * scale, 1.0L / 3)
            || !close_value((*scaled_inverse)[2][2] * scale, 0.25L)){
            return 1;
        }
        const Point3 scaled_right = scaled * point;
        const std::optional<Point3> scaled_solution = scaled.solve(scaled_right);
        if(!scaled_solution || !close_point(*scaled_solution, point)) return 1;
    }

    const long double near_maximum =
        std::numeric_limits<long double>::max() * 0.75L;
    const Matrix3 extreme(Matrix3::Storage{{
        Matrix3::Row{{near_maximum, 0, 0}},
        Matrix3::Row{{0, near_maximum, 0}},
        Matrix3::Row{{0, 0, near_maximum}},
    }});
    const std::optional<Matrix3> extreme_inverse = extreme.inverse();
    if(!extreme_inverse || !close_matrix(
        extreme * *extreme_inverse, identity
    )) return 1;

    // A shared 1 / det / scale factor would overflow here although every
    // element of the inverse is representable.
    constexpr long double tiny_scale = 1e-4925L;
    const Matrix3 anisotropic_tiny(Matrix3::Storage{{
        Matrix3::Row{{tiny_scale, 0, 0}},
        Matrix3::Row{{0, tiny_scale * 1e-5L, 0}},
        Matrix3::Row{{0, 0, tiny_scale * 1e-5L}},
    }});
    const std::optional<Matrix3> anisotropic_inverse =
        anisotropic_tiny.inverse(0.0L);
    if(!anisotropic_inverse || !close_matrix(
        anisotropic_tiny * *anisotropic_inverse, identity
    )) return 1;

    Matrix3 non_finite = identity;
    non_finite[0][0] = std::numeric_limits<long double>::infinity();
    if(non_finite.inverse()) return 1;
    non_finite[0][0] = std::numeric_limits<long double>::quiet_NaN();
    if(non_finite.inverse()) return 1;
    if(identity.inverse(-1.0L)
        || identity.inverse(std::numeric_limits<long double>::infinity())
        || identity.inverse(std::numeric_limits<long double>::quiet_NaN())){
        return 1;
    }

    const Matrix3 relatively_singular(Matrix3::Storage{{
        Matrix3::Row{{1, 0, 0}},
        Matrix3::Row{{0, 1, 0}},
        Matrix3::Row{{0, 0, GEOMETRY3D_EPS / 2}},
    }});
    if(relatively_singular.inverse()) return 1;
    if(!relatively_singular.inverse(0.0L)) return 1;

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
