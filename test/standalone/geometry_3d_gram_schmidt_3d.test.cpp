// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/point_collection/gram_schmidt_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const auto basis = gram_schmidt_3d({
            {2, 0, 0}, {1, 1, 0}, {4, 4, 0}, {1, 2, 3}
        });
        if(basis.size() != 3) return false;
        for(std::size_t i = 0; i < basis.size(); ++i){
            if(!geometry3d_api_close(dot(basis[i], basis[i]), 1)) return false;
            for(std::size_t j = 0; j < i; ++j){
                if(!geometry3d_api_close(dot(basis[i], basis[j]), 0)) return false;
            }
        }
        if(!gram_schmidt_3d({{0, 0, 0}, {1, 0, 0}, {2, 0, 0}}).size()){
            return false;
        }
        bool finite_threw = false;
        try{
            static_cast<void>(gram_schmidt_3d({
                {std::numeric_limits<long double>::infinity(), 0, 0}
            }));
        }catch(const std::invalid_argument&){ finite_threw = true; }
        if(!finite_threw) return false;

        std::uniform_real_distribution<long double> value(-10, 10);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const auto random_basis = gram_schmidt_3d({
                {value(random), value(random), value(random)},
                {value(random), value(random), value(random)},
                {value(random), value(random), value(random)},
            });
            if(random_basis.size() != 3) return false;
            for(std::size_t i = 0; i < 3; ++i){
                for(std::size_t j = 0; j < 3; ++j){
                    if(!geometry3d_api_close(
                        dot(random_basis[i], random_basis[j]), i == j ? 1 : 0,
                        2e-9L
                    )) return false;
                }
            }
        }
        return true;
    });
}
