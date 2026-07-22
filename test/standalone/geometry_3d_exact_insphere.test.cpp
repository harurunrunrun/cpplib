// competitive-verifier: STANDALONE

#include <cstdint>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/exact_insphere.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const IntegerPoint3 a{1, 1, 1}, b{-1, -1, 1};
        const IntegerPoint3 c{-1, 1, -1}, d{1, -1, -1};
        if(exact_insphere(a, b, c, d, {0, 0, 0}) != 1) return false;
        if(exact_insphere(a, b, c, d, a) != 0) return false;
        if(exact_insphere(a, b, c, d, {4, 0, 0}) != -1) return false;
        if(exact_insphere(a, c, b, d, {0, 0, 0}) != 1) return false;
        try{
            static_cast<void>(exact_insphere(
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}
            ));
            return false;
        }catch(const std::invalid_argument&){}
        constexpr std::int64_t scale = std::numeric_limits<std::int64_t>::max();
        const IntegerPoint3 first{scale, scale, scale};
        const IntegerPoint3 second{-scale, -scale, scale};
        const IntegerPoint3 third{-scale, scale, -scale};
        const IntegerPoint3 fourth{scale, -scale, -scale};
        if(exact_insphere(first, second, third, fourth, {0, 0, 0}) != 1
            || exact_insphere(
                first, second, third, fourth, {-scale, -scale, -scale}
            ) != 0) return false;
        return true;
    });
}
