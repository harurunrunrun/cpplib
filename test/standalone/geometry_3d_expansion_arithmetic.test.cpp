// competitive-verifier: STANDALONE

#include <boost/multiprecision/cpp_bin_float.hpp>

#include "../../src/algorithm/geometry/3d/expansion_arithmetic.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{
using HighPrecision = boost::multiprecision::number<
    boost::multiprecision::cpp_bin_float<256>
>;

HighPrecision exact_value(const Geometry3DExpansion& expansion){
    HighPrecision result = 0;
    for(long double component: expansion) result += HighPrecision(component);
    return result;
}
}

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const long double large = 1.0e20L;
        const auto sum = geometry3d_two_sum(large, 1.0L);
        if(exact_value(sum) != HighPrecision(large) + 1) return false;
        const long double first = 1.0e10L + 1.0L;
        const long double second = 1.0e10L - 1.0L;
        const auto product = geometry3d_two_product(first, second);
        if(exact_value(product) != HighPrecision(first) * HighPrecision(second)) return false;
        const auto combined = geometry3d_expansion_sum(sum, product);
        if(exact_value(combined) != exact_value(sum) + exact_value(product)) return false;
        const auto multiplied = geometry3d_expansion_product(sum, product);
        if(exact_value(multiplied) != exact_value(sum) * exact_value(product)) return false;
        if(geometry3d_expansion_sign(multiplied) != 1) return false;
        if(geometry3d_expansion_sign(
            geometry3d_expansion_negate(multiplied)
        ) != -1) return false;
        return true;
    });
}
