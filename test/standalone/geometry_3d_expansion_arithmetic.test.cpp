// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "../../src/algorithm/geometry/3d/expansion_arithmetic.hpp"
#include "../../src/algorithm/math/exact_integer.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

struct ExactDyadicValue{
    ExactInteger significand = 0;
    int exponent = 0;
};

ExactDyadicValue exact_value(long double value){
    if(value == 0.0L) return {};
    const bool negative = std::signbit(value);
    int exponent = 0;
    long double fraction = std::frexp(std::abs(value), &exponent);
    constexpr int digits = std::numeric_limits<long double>::digits;
    ExactInteger significand = 0;
    for(int bit = 0; bit < digits; ++bit){
        fraction = std::ldexp(fraction, 1);
        significand <<= 1;
        if(fraction >= 1.0L){
            significand += 1;
            fraction -= 1.0L;
        }
    }
    if(negative) significand = -significand;
    return {std::move(significand), exponent - digits};
}

ExactDyadicValue add(
    const ExactDyadicValue& first,
    const ExactDyadicValue& second
){
    if(first.significand == 0) return second;
    if(second.significand == 0) return first;
    const int exponent = std::min(first.exponent, second.exponent);
    return {
        (first.significand << static_cast<std::size_t>(first.exponent - exponent))
            + (second.significand << static_cast<std::size_t>(
                second.exponent - exponent
            )),
        exponent,
    };
}

ExactDyadicValue multiply(
    const ExactDyadicValue& first,
    const ExactDyadicValue& second
){
    return {
        first.significand * second.significand,
        first.exponent + second.exponent,
    };
}

ExactDyadicValue exact_value(const Geometry3DExpansion& expansion){
    ExactDyadicValue result;
    for(long double component: expansion){
        result = add(result, exact_value(component));
    }
    return result;
}

bool equal(const ExactDyadicValue& first, const ExactDyadicValue& second){
    return add(first, {
        -second.significand, second.exponent
    }).significand == 0;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const long double large = 1.0e20L;
        const auto sum = geometry3d_two_sum(large, 1.0L);
        if(!equal(exact_value(sum), add(exact_value(large), exact_value(1.0L)))){
            return false;
        }
        const long double first = 1.0e10L + 1.0L;
        const long double second = 1.0e10L - 1.0L;
        const auto product = geometry3d_two_product(first, second);
        if(!equal(
            exact_value(product),
            multiply(exact_value(first), exact_value(second))
        )) return false;
        const auto combined = geometry3d_expansion_sum(sum, product);
        if(!equal(
            exact_value(combined),
            add(exact_value(sum), exact_value(product))
        )) return false;
        const auto multiplied = geometry3d_expansion_product(sum, product);
        if(!equal(
            exact_value(multiplied),
            multiply(exact_value(sum), exact_value(product))
        )) return false;
        if(geometry3d_expansion_sign(multiplied) != 1) return false;
        if(geometry3d_expansion_sign(
            geometry3d_expansion_negate(multiplied)
        ) != -1) return false;
        return true;
    });
}
