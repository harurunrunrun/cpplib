#ifndef CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "convolution.hpp"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace formal_power_series_internal{

template<int MOD>
using mint = Modint<MOD>;

inline void check_target_size(std::size_t size, const char* function_name){
    if(size > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(function_name);
    }
}

template<int MOD>
std::vector<mint<MOD>> prefix(const std::vector<mint<MOD>>& values, std::size_t size){
    return std::vector<mint<MOD>>(
        values.begin(), values.begin() + static_cast<std::ptrdiff_t>(std::min(values.size(), size))
    );
}

template<int MOD>
std::vector<mint<MOD>> multiply_truncated(
    const std::vector<mint<MOD>>& left,
    const std::vector<mint<MOD>>& right,
    std::size_t size
){
    if(size == 0 || left.empty() || right.empty()) return {};
    std::vector<mint<MOD>> truncated_left = prefix<MOD>(left, size);
    std::vector<mint<MOD>> truncated_right = prefix<MOD>(right, size);
    const std::size_t need = truncated_left.size() + truncated_right.size() - 1;
    std::size_t transform_size = 1;
    while(transform_size < need){
        if(transform_size > static_cast<std::size_t>(std::numeric_limits<int>::max()) / 2)[[unlikely]]{
            throw std::length_error("formal power series convolution size is too large");
        }
        transform_size *= 2;
    }
    if(static_cast<std::size_t>(MOD - 1) % transform_size != 0)[[unlikely]]{
        throw std::runtime_error("formal power series exceeds the NTT capacity of the modulus");
    }
    std::vector<mint<MOD>> result = convolution_ntt<MOD>(
        std::move(truncated_left), std::move(truncated_right)
    );
    if(result.size() > size) result.resize(size);
    return result;
}

template<int MOD>
std::optional<mint<MOD>> modular_sqrt(mint<MOD> value){
    static_assert(MOD >= 2);
    if(value == mint<MOD>(0)) return mint<MOD>(0);
    if constexpr(MOD == 2) return value;
    if(value.pow((MOD - 1LL) / 2) != mint<MOD>(1)) return std::nullopt;
    if constexpr(MOD % 4 == 3){
        mint<MOD> root = value.pow((MOD + 1LL) / 4);
        if(MOD - root.val() < root.val()) root = -root;
        return root;
    }

    long long odd = MOD - 1LL;
    int exponent_of_two = 0;
    while((odd & 1) == 0){ odd >>= 1; ++exponent_of_two; }
    mint<MOD> non_residue = 2;
    while(non_residue.pow((MOD - 1LL) / 2) == mint<MOD>(1)) ++non_residue;
    mint<MOD> root = value.pow((odd + 1) / 2);
    mint<MOD> remainder = value.pow(odd);
    mint<MOD> base = non_residue.pow(odd);
    int order = exponent_of_two;
    while(remainder != mint<MOD>(1)){
        int index = 1;
        mint<MOD> squared = remainder * remainder;
        while(index < order && squared != mint<MOD>(1)){
            squared *= squared;
            ++index;
        }
        if(index == order) return std::nullopt;
        const mint<MOD> multiplier = base.pow(1LL << (order - index - 1));
        root *= multiplier;
        base = multiplier * multiplier;
        remainder *= base;
        order = index;
    }
    if(MOD - root.val() < root.val()) root = -root;
    return root;
}

} // namespace formal_power_series_internal

template<int MOD>
std::vector<Modint<MOD>> fps_derivative(const std::vector<Modint<MOD>>& values){
    if(values.empty()) return {};
    std::vector<Modint<MOD>> result(values.size() - 1);
    for(std::size_t i = 1; i < values.size(); ++i){
        result[i - 1] = values[i] * Modint<MOD>(static_cast<long long>(i % MOD));
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> fps_integral(const std::vector<Modint<MOD>>& values){
    static_assert(MOD >= 2);
    std::vector<Modint<MOD>> result(values.size() + 1);
    for(std::size_t i = 0; i < values.size(); ++i){
        const Modint<MOD> denominator(static_cast<long long>((i + 1) % MOD));
        result[i + 1] = values[i] / denominator;
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> fps_inv(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
){
    static_assert(MOD >= 2);
    formal_power_series_internal::check_target_size(
        size, "formal power series inverse target size is too large"
    );
    if(size == 0) return {};
    if(values.empty() || values[0] == Modint<MOD>(0))[[unlikely]]{
        throw std::runtime_error("formal power series inverse requires a nonzero constant term");
    }
    std::vector<Modint<MOD>> result{values[0].inv()};
    while(result.size() < size){
        const std::size_t next_size = std::min(size, result.size() * 2);
        std::vector<Modint<MOD>> product =
            formal_power_series_internal::multiply_truncated<MOD>(
                formal_power_series_internal::prefix<MOD>(values, next_size),
                result,
                next_size
            );
        product.resize(next_size);
        for(Modint<MOD>& value: product) value = -value;
        product[0] += Modint<MOD>(2);
        result = formal_power_series_internal::multiply_truncated<MOD>(
            result, product, next_size
        );
        result.resize(next_size);
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> fps_log(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
){
    static_assert(MOD >= 2);
    formal_power_series_internal::check_target_size(
        size, "formal power series logarithm target size is too large"
    );
    if(size == 0) return {};
    if(values.empty() || values[0] != Modint<MOD>(1))[[unlikely]]{
        throw std::runtime_error("formal power series logarithm requires constant term one");
    }
    std::vector<Modint<MOD>> derivative = fps_derivative(
        formal_power_series_internal::prefix<MOD>(values, size)
    );
    std::vector<Modint<MOD>> inverse = fps_inv(values, size);
    std::vector<Modint<MOD>> product =
        formal_power_series_internal::multiply_truncated<MOD>(
            derivative, inverse, size - 1
        );
    product.resize(size - 1);
    std::vector<Modint<MOD>> result = fps_integral(product);
    result.resize(size);
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> fps_exp(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
){
    static_assert(MOD >= 2);
    formal_power_series_internal::check_target_size(
        size, "formal power series exponential target size is too large"
    );
    if(size == 0) return {};
    if(!values.empty() && values[0] != Modint<MOD>(0))[[unlikely]]{
        throw std::runtime_error("formal power series exponential requires constant term zero");
    }
    std::vector<Modint<MOD>> result{Modint<MOD>(1)};
    while(result.size() < size){
        const std::size_t next_size = std::min(size, result.size() * 2);
        const std::vector<Modint<MOD>> logarithm = fps_log(result, next_size);
        std::vector<Modint<MOD>> correction(next_size);
        for(std::size_t i = 0; i < next_size; ++i){
            const Modint<MOD> source = i < values.size() ? values[i] : Modint<MOD>(0);
            correction[i] = source - logarithm[i];
        }
        correction[0] += Modint<MOD>(1);
        result = formal_power_series_internal::multiply_truncated<MOD>(
            result, correction, next_size
        );
        result.resize(next_size);
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> fps_pow(
    const std::vector<Modint<MOD>>& values,
    long long exponent,
    std::size_t size
){
    static_assert(MOD >= 2);
    if(exponent < 0)[[unlikely]]{
        throw std::runtime_error("formal power series power requires a nonnegative exponent");
    }
    formal_power_series_internal::check_target_size(
        size, "formal power series power target size is too large"
    );
    if(size == 0) return {};
    std::vector<Modint<MOD>> result(size);
    if(exponent == 0){ result[0] = Modint<MOD>(1); return result; }

    const std::size_t input_size = std::min(values.size(), size);
    std::size_t first = 0;
    while(first < input_size && values[first] == Modint<MOD>(0)) ++first;
    if(first == input_size) return result;
    if(first != 0 && static_cast<unsigned long long>(exponent) > (size - 1) / first){
        return result;
    }
    const std::size_t shift = first * static_cast<std::size_t>(exponent);
    const std::size_t unit_size = size - shift;
    const Modint<MOD> constant = values[first];
    const Modint<MOD> inverse_constant = constant.inv();
    std::vector<Modint<MOD>> unit(
        values.begin() + static_cast<std::ptrdiff_t>(first),
        values.begin() + static_cast<std::ptrdiff_t>(input_size)
    );
    for(Modint<MOD>& value: unit) value *= inverse_constant;
    std::vector<Modint<MOD>> logarithm = fps_log(unit, unit_size);
    const Modint<MOD> scalar(exponent);
    for(Modint<MOD>& value: logarithm) value *= scalar;
    std::vector<Modint<MOD>> powered = fps_exp(logarithm, unit_size);
    const Modint<MOD> constant_power = constant.pow(exponent);
    for(std::size_t i = 0; i < unit_size; ++i){
        result[shift + i] = powered[i] * constant_power;
    }
    return result;
}

template<int MOD>
std::optional<std::vector<Modint<MOD>>> fps_sqrt(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
){
    static_assert(MOD >= 2);
    formal_power_series_internal::check_target_size(
        size, "formal power series square root target size is too large"
    );
    if(size == 0) return std::vector<Modint<MOD>>{};
    const std::size_t input_size = std::min(values.size(), size);
    std::size_t first = 0;
    while(first < input_size && values[first] == Modint<MOD>(0)) ++first;
    if(first == input_size) return std::vector<Modint<MOD>>(size);
    if((first & 1) != 0) return std::nullopt;
    if constexpr(MOD == 2){
        throw std::runtime_error("formal power series square root requires odd characteristic");
    }
    const std::optional<Modint<MOD>> constant_root =
        formal_power_series_internal::modular_sqrt<MOD>(values[first]);
    if(!constant_root) return std::nullopt;

    const std::size_t shift = first / 2;
    const std::size_t unit_size = size - shift;
    const Modint<MOD> inverse_constant = values[first].inv();
    std::vector<Modint<MOD>> unit(
        values.begin() + static_cast<std::ptrdiff_t>(first),
        values.begin() + static_cast<std::ptrdiff_t>(input_size)
    );
    for(Modint<MOD>& value: unit) value *= inverse_constant;
    std::vector<Modint<MOD>> root{Modint<MOD>(1)};
    const Modint<MOD> inverse_two = Modint<MOD>(2).inv();
    while(root.size() < unit_size){
        const std::size_t next_size = std::min(unit_size, root.size() * 2);
        const std::vector<Modint<MOD>> inverse = fps_inv(root, next_size);
        std::vector<Modint<MOD>> quotient =
            formal_power_series_internal::multiply_truncated<MOD>(
                formal_power_series_internal::prefix<MOD>(unit, next_size),
                inverse,
                next_size
            );
        quotient.resize(next_size);
        root.resize(next_size);
        for(std::size_t i = 0; i < next_size; ++i){
            root[i] = (root[i] + quotient[i]) * inverse_two;
        }
    }

    std::vector<Modint<MOD>> result(size);
    for(std::size_t i = 0; i < unit_size; ++i){
        result[shift + i] = root[i] * *constant_root;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_HPP_INCLUDED
