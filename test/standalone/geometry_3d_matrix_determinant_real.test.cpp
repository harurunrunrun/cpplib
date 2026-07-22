// competitive-verifier: STANDALONE

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/scalar/matrix_determinant_real.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(matrix_determinant_real({}), 1)) return false;
        if(!geometry3d_api_close(matrix_determinant_real({
            {1, 2, 3}, {0, 4, 5}, {1, 0, 6}
        }), 22)) return false;
        if(matrix_determinant_real({{1, 2}, {2, 4}}) != 0) return false;
        bool square_threw = false;
        try{
            static_cast<void>(matrix_determinant_real({{1, 2, 3}, {4, 5, 6}}));
        }catch(const std::invalid_argument&){ square_threw = true; }
        if(!square_threw) return false;

        std::uniform_real_distribution<long double> value(-10, 10);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<std::vector<long double>> matrix(3, std::vector<long double>(3));
            for(auto& row: matrix) for(long double& entry: row) entry = value(random);
            const long double expected =
                matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
                matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
                matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
            const long double actual = matrix_determinant_real(matrix, 1e-15L);
            if(!geometry3d_api_close(actual, expected, 2e-10L)) return false;
        }
        return true;
    });
}
