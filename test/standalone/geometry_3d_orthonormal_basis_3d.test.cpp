// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/point_collection/orthonormal_basis_3d.hpp"
#include "../../src/algorithm/geometry/3d/scalar/dot.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        bool zero_threw = false;
        try{
            static_cast<void>(orthonormal_basis_3d({0, 0, 0}));
        }catch(const std::invalid_argument&){ zero_threw = true; }
        if(!zero_threw) return false;

        std::uniform_real_distribution<long double> value(-10, 10);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Point3 direction{value(random), value(random), value(random)};
            if(direction == Point3{}) direction = {1, 0, 0};
            const auto basis = orthonormal_basis_3d(direction);
            if(dot(basis[0], direction) <= 0) return false;
            for(std::size_t i = 0; i < 3; ++i){
                for(std::size_t j = 0; j < 3; ++j){
                    if(!geometry3d_api_close(
                        dot(basis[i], basis[j]), i == j ? 1 : 0, 2e-10L
                    )) return false;
                }
            }
        }
        return true;
    });
}
