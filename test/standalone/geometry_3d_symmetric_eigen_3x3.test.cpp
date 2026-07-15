// competitive-verifier: STANDALONE

#include <cstddef>
#include <random>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/symmetric_eigen_3x3.hpp"
#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        Matrix3 basic;
        basic[0] = {{2, 1, 0}};
        basic[1] = {{1, 2, 0}};
        basic[2] = {{0, 0, 4}};
        const SymmetricEigen3 decomposition = symmetric_eigen_3x3(basic);
        if(!geometry3d_api_close(decomposition.eigenvalues[0], 4) ||
           !geometry3d_api_close(decomposition.eigenvalues[1], 3) ||
           !geometry3d_api_close(decomposition.eigenvalues[2], 1)) return false;
        const long double maximum = std::numeric_limits<long double>::max();
        Matrix3 huge;
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = 0; column < 3; ++column){
                huge[row][column] = maximum;
            }
        }
        bool overflow_threw = false;
        try{
            static_cast<void>(symmetric_eigen_3x3(huge));
        }catch(const std::overflow_error&){ overflow_threw = true; }
        if(!overflow_threw) return false;
        bool symmetric_threw = false;
        try{
            Matrix3 invalid = basic;
            invalid[0][1] = 2;
            static_cast<void>(symmetric_eigen_3x3(invalid));
        }catch(const std::invalid_argument&){ symmetric_threw = true; }
        if(!symmetric_threw) return false;

        std::uniform_real_distribution<long double> value(-10, 10);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Matrix3 matrix;
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = row; column < 3; ++column){
                    matrix[row][column] = matrix[column][row] = value(random);
                }
            }
            const SymmetricEigen3 eigen = symmetric_eigen_3x3(matrix, 1e-13L);
            for(std::size_t index = 0; index < 3; ++index){
                const Point3 vector = eigen.eigenvector(index);
                if(!geometry3d_api_close(dot(vector, vector), 1, 2e-9L) ||
                   !geometry3d_api_close(
                       matrix * vector, vector * eigen.eigenvalues[index], 2e-8L
                   )) return false;
                for(std::size_t previous = 0; previous < index; ++previous){
                    if(!geometry3d_api_close(
                        dot(vector, eigen.eigenvector(previous)), 0, 2e-9L
                    )) return false;
                }
            }
        }
        return true;
    });
}
