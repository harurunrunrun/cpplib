#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CHIRP_Z_TRANSFORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CHIRP_Z_TRANSFORM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <vector>

#include "../../math/polynomial/polynomial_middle_product.hpp"

template<int MOD>
std::vector<Modint<MOD>> chirp_z_transform(
    const std::vector<Modint<MOD>>& coefficients,
    const Modint<MOD> initial_point,
    const Modint<MOD> ratio,
    const std::size_t count
){
    using mint = Modint<MOD>;
    if(count == 0) return {};
    if(coefficients.empty()) return std::vector<mint>(count);
    if(ratio == mint(0)){
        std::vector<mint> result(count, coefficients[0]);
        mint point_power = 1;
        result[0] = 0;
        for(const mint coefficient: coefficients){
            result[0] += coefficient * point_power;
            point_power *= initial_point;
        }
        return result;
    }

    const std::size_t total = coefficients.size() + count - 1;
    std::vector<mint> chirp(total);
    std::vector<mint> inverse_chirp(
        std::max(coefficients.size(), count)
    );
    chirp[0] = 1;
    inverse_chirp[0] = 1;
    mint ratio_power = 1;
    mint inverse_ratio_power = 1;
    const mint inverse_ratio = ratio.inv();
    for(std::size_t index = 1; index < total; ++index){
        chirp[index] = chirp[index - 1] * ratio_power;
        ratio_power *= ratio;
        if(index < inverse_chirp.size()){
            inverse_chirp[index] =
                inverse_chirp[index - 1] * inverse_ratio_power;
            inverse_ratio_power *= inverse_ratio;
        }
    }

    std::vector<mint> weighted(coefficients.size());
    mint point_power = 1;
    for(std::size_t index = 0; index < coefficients.size(); ++index){
        weighted[index] = coefficients[index] * point_power
            * inverse_chirp[index];
        point_power *= initial_point;
    }
    std::vector<mint> result = math::polynomial_middle_product<MOD>(
        chirp, weighted
    );
    result.resize(count);
    for(std::size_t index = 0; index < count; ++index){
        result[index] *= inverse_chirp[index];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CHIRP_Z_TRANSFORM_HPP_INCLUDED
