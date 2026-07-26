// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/gaussian_elimination_real.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const auto reduced = gaussian_elimination_real({
            {1, 2, 3}, {2, 4, 6}, {0, 1, 1}
        });
        if(reduced.rank() != 2 || reduced.pivot_columns != std::vector<std::size_t>{0, 1}){
            return false;
        }
        const auto solution = solve_square_linear_system_real(
            {{3, 1}, {1, 2}}, {9, 8}
        );
        if(!solution || !geometry3d_api_close((*solution)[0], 2) ||
           !geometry3d_api_close((*solution)[1], 3)) return false;
        if(solve_square_linear_system_real({{1, 2}, {2, 4}}, {1, 2})) return false;

        bool ragged_threw = false;
        try{
            static_cast<void>(gaussian_elimination_real({{1, 2}, {3}}));
        }catch(const std::invalid_argument&){ ragged_threw = true; }
        if(!ragged_threw) return false;
        bool finite_threw = false;
        try{
            static_cast<void>(gaussian_elimination_real({
                {std::numeric_limits<long double>::infinity()}
            }));
        }catch(const std::invalid_argument&){ finite_threw = true; }
        if(!finite_threw) return false;

        std::uniform_real_distribution<long double> value(-5, 5);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<std::vector<long double>> matrix(3, std::vector<long double>(3));
            std::vector<long double> expected(3), rhs(3);
            for(std::size_t row = 0; row < 3; ++row){
                expected[row] = value(random);
                for(std::size_t column = 0; column < 3; ++column){
                    matrix[row][column] = value(random);
                }
                matrix[row][row] += matrix[row][row] < 0 ? -15 : 15;
            }
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = 0; column < 3; ++column){
                    rhs[row] += matrix[row][column] * expected[column];
                }
            }
            const auto actual = solve_square_linear_system_real(matrix, rhs);
            if(!actual) return false;
            for(std::size_t index = 0; index < 3; ++index){
                if(!geometry3d_api_close((*actual)[index], expected[index], 2e-9L)){
                    return false;
                }
            }
        }
        return true;
    });
}
