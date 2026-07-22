// competitive-verifier: STANDALONE

#include <cstddef>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/scalar/matrix_rank_real.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(matrix_rank_real({}) != 0 || matrix_rank_real({{}, {}}) != 0) return false;
        if(matrix_rank_real({{1, 2, 3}, {2, 4, 6}, {0, 1, 1}}) != 2) return false;
        if(matrix_rank_real({{1e3000L, 0}, {0, 1e3000L}}) != 2) return false;
        if(matrix_rank_real({{1, 1}, {1, 1 + 1e-13L}}) != 1) return false;
        if(matrix_rank_real({{1, 1}, {1, 1 + 1e-6L}}) != 2) return false;

        std::uniform_real_distribution<long double> value(-10, 10);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<long double> left(4), right(5);
            for(long double& entry: left) entry = value(random);
            for(long double& entry: right) entry = value(random);
            std::vector<std::vector<long double>> outer(4, std::vector<long double>(5));
            for(std::size_t row = 0; row < 4; ++row){
                for(std::size_t column = 0; column < 5; ++column){
                    outer[row][column] = left[row] * right[column];
                }
            }
            if(matrix_rank_real(outer, 1e-12L) != 1) return false;
        }
        return true;
    });
}
