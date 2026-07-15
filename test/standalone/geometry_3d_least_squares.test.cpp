// competitive-verifier: STANDALONE

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/least_squares.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const std::vector<std::vector<long double>> design{
            {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}
        };
        const auto line = least_squares(design, {-4, -1, 2, 5, 8});
        if(!line || !geometry3d_api_close((*line)[0], 2) ||
           !geometry3d_api_close((*line)[1], 3)) return false;
        if(least_squares({{1, 1}, {2, 2}}, {1, 2})) return false;
        if(least_squares({{1, 0, 0}, {0, 1, 0}}, {1, 2})) return false;
        const auto empty = least_squares({{}, {}, {}}, {1, 2, 3});
        if(!empty || !empty->empty()) return false;
        bool shape_threw = false;
        try{
            static_cast<void>(least_squares({{1}, {2}}, {1}));
        }catch(const std::invalid_argument&){ shape_threw = true; }
        if(!shape_threw) return false;

        std::uniform_real_distribution<long double> value(-5, 5);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<std::vector<long double>> matrix(8, std::vector<long double>(3));
            std::vector<long double> expected(3), rhs(8);
            for(long double& entry: expected) entry = value(random);
            for(std::size_t row = 0; row < matrix.size(); ++row){
                matrix[row][0] = 1;
                matrix[row][1] = value(random);
                matrix[row][2] = value(random);
                for(std::size_t column = 0; column < 3; ++column){
                    rhs[row] += matrix[row][column] * expected[column];
                }
            }
            const auto actual = least_squares(matrix, rhs);
            if(!actual) return false;
            for(std::size_t column = 0; column < 3; ++column){
                if(!geometry3d_api_close((*actual)[column], expected[column], 3e-8L)){
                    return false;
                }
            }
        }
        return true;
    });
}
