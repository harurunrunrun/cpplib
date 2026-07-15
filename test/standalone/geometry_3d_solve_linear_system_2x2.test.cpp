// competitive-verifier: STANDALONE

#include <array>
#include <cstddef>
#include <random>

#include "../../src/algorithm/geometry/3d/solve_linear_system_2x2.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const auto basic = solve_linear_system_2x2(
            {{{3, 1}, {1, 2}}}, {{9, 8}}
        );
        if(!basic || !geometry3d_api_close((*basic)[0], 2) ||
           !geometry3d_api_close((*basic)[1], 3)) return false;
        if(solve_linear_system_2x2({{{1, 2}, {2, 4}}}, {{3, 6}})) return false;
        const long double scale = 1e3000L;
        const auto large = solve_linear_system_2x2(
            {{{scale, 0}, {0, scale}}}, {{2 * scale, -3 * scale}}
        );
        if(!large || !geometry3d_api_close((*large)[0], 2) ||
           !geometry3d_api_close((*large)[1], -3)) return false;

        std::uniform_real_distribution<long double> value(-20, 20);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::array<std::array<long double, 2>, 2> matrix{{
                {{value(random) + 30, value(random)}},
                {{value(random), value(random) - 30}},
            }};
            const std::array<long double, 2> expected{{value(random), value(random)}};
            const std::array<long double, 2> rhs{{
                matrix[0][0] * expected[0] + matrix[0][1] * expected[1],
                matrix[1][0] * expected[0] + matrix[1][1] * expected[1],
            }};
            const auto actual = solve_linear_system_2x2(matrix, rhs);
            if(!actual || !geometry3d_api_close((*actual)[0], expected[0], 2e-9L) ||
               !geometry3d_api_close((*actual)[1], expected[1], 2e-9L)) return false;
        }
        return true;
    });
}
