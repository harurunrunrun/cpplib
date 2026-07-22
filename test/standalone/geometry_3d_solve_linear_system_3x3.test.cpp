// competitive-verifier: STANDALONE

#include <cstddef>
#include <random>

#include "../../src/algorithm/geometry/3d/query/solve_linear_system_3x3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Matrix3 matrix(Matrix3::Storage{{
            Matrix3::Row{{4, 1, -1}},
            Matrix3::Row{{2, 7, 1}},
            Matrix3::Row{{1, -3, 12}},
        }});
        const Point3 expected{2, -1, 3};
        const auto basic = solve_linear_system_3x3(matrix, matrix * expected);
        if(!basic || !geometry3d_api_close(*basic, expected)) return false;
        Matrix3 singular;
        singular[0] = {{1, 2, 3}};
        singular[1] = {{2, 4, 6}};
        singular[2] = {{0, 1, 1}};
        if(solve_linear_system_3x3(singular, {1, 2, 3})) return false;

        std::uniform_real_distribution<long double> value(-5, 5);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Matrix3 current;
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = 0; column < 3; ++column){
                    current[row][column] = value(random);
                }
                current[row][row] += 15;
            }
            const Point3 target{value(random), value(random), value(random)};
            const auto actual = solve_linear_system_3x3(current, current * target);
            if(!actual || !geometry3d_api_close(*actual, target, 2e-9L)) return false;
        }
        return true;
    });
}
