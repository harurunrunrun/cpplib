#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/modint/modint.hpp"

template<int MOD>
using SparseFormalPowerSeriesTerm = std::pair<std::size_t, Modint<MOD>>;

template<int MOD>
std::vector<SparseFormalPowerSeriesTerm<MOD>> sparse_fps_normalize(
    std::vector<SparseFormalPowerSeriesTerm<MOD>> terms
){
    std::sort(terms.begin(), terms.end(), [](const auto& left, const auto& right){
        return left.first < right.first;
    });
    std::vector<SparseFormalPowerSeriesTerm<MOD>> result;
    result.reserve(terms.size());
    for(const auto& [degree, coefficient]: terms){
        if(coefficient == Modint<MOD>(0)) continue;
        if(!result.empty() && result.back().first == degree){
            result.back().second += coefficient;
            if(result.back().second == Modint<MOD>(0)) result.pop_back();
        }else{
            result.emplace_back(degree, coefficient);
        }
    }
    return result;
}

namespace sparse_formal_power_series_internal{

constexpr bool is_prime(int value){
    if(value < 2) return false;
    if(value % 2 == 0) return value == 2;
    for(long long divisor = 3; divisor * divisor <= value; divisor += 2){
        if(value % static_cast<int>(divisor) == 0) return false;
    }
    return true;
}

template<int MOD>
Modint<MOD> from_size(std::size_t value){
    return Modint<MOD>(static_cast<long long>(value % static_cast<std::size_t>(MOD)));
}

template<int MOD>
std::vector<Modint<MOD>> inverse_degrees(std::size_t size){
    std::vector<Modint<MOD>> result(size);
    if(size <= 1) return result;
    result[1] = 1;
    for(std::size_t value = 2; value < size; ++value){
        const std::size_t quotient = static_cast<std::size_t>(MOD) / value;
        const std::size_t remainder = static_cast<std::size_t>(MOD) % value;
        result[value] = -from_size<MOD>(quotient) * result[remainder];
    }
    return result;
}

template<int MOD>
void check_characteristic(std::size_t size, const char* function_name){
    static_assert(is_prime(MOD), "sparse formal power series requires a prime modulus");
    if(size > static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::runtime_error(function_name);
    }
}

template<int MOD>
std::vector<SparseFormalPowerSeriesTerm<MOD>> normalized_prefix(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
){
    std::vector<SparseFormalPowerSeriesTerm<MOD>> result = sparse_fps_normalize<MOD>(terms);
    const auto end = std::lower_bound(
        result.begin(), result.end(), size,
        [](const auto& term, std::size_t bound){ return term.first < bound; }
    );
    result.erase(end, result.end());
    return result;
}

template<int MOD>
std::optional<Modint<MOD>> modular_sqrt(Modint<MOD> value){
    static_assert(is_prime(MOD), "modular square root requires a prime modulus");
    if(value == Modint<MOD>(0)) return Modint<MOD>(0);
    if constexpr(MOD == 2) return value;
    if(value.pow((MOD - 1LL) / 2) != Modint<MOD>(1)) return std::nullopt;
    if constexpr(MOD % 4 == 3){
        Modint<MOD> root = value.pow((MOD + 1LL) / 4);
        if(MOD - root.val() < root.val()) root = -root;
        return root;
    }

    long long odd = MOD - 1LL;
    int exponent_of_two = 0;
    while((odd & 1LL) == 0){
        odd >>= 1;
        ++exponent_of_two;
    }
    Modint<MOD> non_residue = 2;
    while(non_residue.pow((MOD - 1LL) / 2) == Modint<MOD>(1)) ++non_residue;
    Modint<MOD> root = value.pow((odd + 1) / 2);
    Modint<MOD> remainder = value.pow(odd);
    Modint<MOD> base = non_residue.pow(odd);
    int order = exponent_of_two;
    while(remainder != Modint<MOD>(1)){
        int index = 1;
        Modint<MOD> squared = remainder * remainder;
        while(index < order && squared != Modint<MOD>(1)){
            squared *= squared;
            ++index;
        }
        if(index == order) return std::nullopt;
        const Modint<MOD> multiplier = base.pow(1LL << (order - index - 1));
        root *= multiplier;
        base = multiplier * multiplier;
        remainder *= base;
        order = index;
    }
    if(MOD - root.val() < root.val()) root = -root;
    return root;
}

} // namespace sparse_formal_power_series_internal

template<int MOD>
std::vector<Modint<MOD>> sparse_fps_inv(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
){
    static_assert(sparse_formal_power_series_internal::is_prime(MOD),
                  "sparse formal power series requires a prime modulus");
    if(size == 0) return {};
    const auto normalized =
        sparse_formal_power_series_internal::normalized_prefix<MOD>(terms, size);
    if(normalized.empty() || normalized.front().first != 0)[[unlikely]]{
        throw std::runtime_error(
            "sparse formal power series inverse requires a nonzero constant term"
        );
    }

    std::vector<Modint<MOD>> result(size);
    const Modint<MOD> inverse_constant = normalized.front().second.inv();
    result[0] = inverse_constant;
    for(std::size_t degree = 1; degree < size; ++degree){
        Modint<MOD> sum = 0;
        for(auto iterator = normalized.begin() + 1; iterator != normalized.end(); ++iterator){
            if(iterator->first > degree) break;
            sum += iterator->second * result[degree - iterator->first];
        }
        result[degree] = -sum * inverse_constant;
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> sparse_fps_log(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
){
    if(size == 0) return {};
    sparse_formal_power_series_internal::check_characteristic<MOD>(
        size, "sparse formal power series logarithm requires target size at most MOD"
    );
    const auto normalized =
        sparse_formal_power_series_internal::normalized_prefix<MOD>(terms, size);
    if(normalized.empty() || normalized.front().first != 0 ||
       normalized.front().second != Modint<MOD>(1))[[unlikely]]{
        throw std::runtime_error(
            "sparse formal power series logarithm requires constant term one"
        );
    }

    const auto inverse_degree =
        sparse_formal_power_series_internal::inverse_degrees<MOD>(size);
    std::vector<Modint<MOD>> result(size);
    for(std::size_t degree = 1; degree < size; ++degree){
        Modint<MOD> numerator = 0;
        for(auto iterator = normalized.begin() + 1; iterator != normalized.end(); ++iterator){
            const std::size_t term_degree = iterator->first;
            if(term_degree > degree) break;
            if(term_degree == degree){
                numerator += sparse_formal_power_series_internal::from_size<MOD>(degree) *
                             iterator->second;
            }else{
                numerator -= iterator->second *
                             sparse_formal_power_series_internal::from_size<MOD>(
                                 degree - term_degree
                             ) * result[degree - term_degree];
            }
        }
        result[degree] = numerator * inverse_degree[degree];
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> sparse_fps_exp(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
){
    if(size == 0) return {};
    sparse_formal_power_series_internal::check_characteristic<MOD>(
        size, "sparse formal power series exponential requires target size at most MOD"
    );
    const auto normalized =
        sparse_formal_power_series_internal::normalized_prefix<MOD>(terms, size);
    if(!normalized.empty() && normalized.front().first == 0)[[unlikely]]{
        throw std::runtime_error(
            "sparse formal power series exponential requires constant term zero"
        );
    }

    const auto inverse_degree =
        sparse_formal_power_series_internal::inverse_degrees<MOD>(size);
    std::vector<Modint<MOD>> result(size);
    result[0] = 1;
    for(std::size_t degree = 1; degree < size; ++degree){
        Modint<MOD> numerator = 0;
        for(const auto& [term_degree, coefficient]: normalized){
            if(term_degree > degree) break;
            numerator += sparse_formal_power_series_internal::from_size<MOD>(term_degree) *
                         coefficient * result[degree - term_degree];
        }
        result[degree] = numerator * inverse_degree[degree];
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> sparse_fps_pow(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    long long exponent,
    std::size_t size
){
    static_assert(sparse_formal_power_series_internal::is_prime(MOD),
                  "sparse formal power series requires a prime modulus");
    if(exponent < 0)[[unlikely]]{
        throw std::runtime_error("sparse formal power series power requires a nonnegative exponent");
    }
    if(size == 0) return {};
    sparse_formal_power_series_internal::check_characteristic<MOD>(
        size, "sparse formal power series power requires target size at most MOD"
    );
    std::vector<Modint<MOD>> result(size);
    if(exponent == 0){
        result[0] = 1;
        return result;
    }

    const auto normalized =
        sparse_formal_power_series_internal::normalized_prefix<MOD>(terms, size);
    if(normalized.empty()) return result;
    const std::size_t first_degree = normalized.front().first;
    const auto unsigned_exponent = static_cast<unsigned long long>(exponent);
    if(first_degree != 0 && unsigned_exponent > (size - 1) / first_degree) return result;
    const std::size_t shift = first_degree * static_cast<std::size_t>(unsigned_exponent);
    const std::size_t unit_size = size - shift;
    const Modint<MOD> leading_coefficient = normalized.front().second;
    const Modint<MOD> inverse_leading = leading_coefficient.inv();

    std::vector<SparseFormalPowerSeriesTerm<MOD>> unit;
    unit.reserve(normalized.size());
    for(const auto& [degree, coefficient]: normalized){
        const std::size_t shifted_degree = degree - first_degree;
        if(shifted_degree >= unit_size) break;
        unit.emplace_back(shifted_degree, coefficient * inverse_leading);
    }

    const auto inverse_degree =
        sparse_formal_power_series_internal::inverse_degrees<MOD>(unit_size);
    std::vector<Modint<MOD>> powered(unit_size);
    powered[0] = 1;
    const Modint<MOD> exponent_mod(exponent);
    for(std::size_t degree = 1; degree < unit_size; ++degree){
        Modint<MOD> numerator = 0;
        for(auto iterator = unit.begin() + 1; iterator != unit.end(); ++iterator){
            const std::size_t term_degree = iterator->first;
            if(term_degree > degree) break;
            const Modint<MOD> term_degree_mod =
                sparse_formal_power_series_internal::from_size<MOD>(term_degree);
            const Modint<MOD> multiplier =
                (exponent_mod + Modint<MOD>(1)) * term_degree_mod -
                sparse_formal_power_series_internal::from_size<MOD>(degree);
            numerator += multiplier * iterator->second * powered[degree - term_degree];
        }
        powered[degree] = numerator * inverse_degree[degree];
    }

    const Modint<MOD> leading_power = leading_coefficient.pow(exponent);
    for(std::size_t degree = 0; degree < unit_size; ++degree){
        result[shift + degree] = powered[degree] * leading_power;
    }
    return result;
}

template<int MOD>
std::optional<std::vector<Modint<MOD>>> sparse_fps_sqrt(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
){
    static_assert(sparse_formal_power_series_internal::is_prime(MOD),
                  "sparse formal power series requires a prime modulus");
    if(size == 0) return std::vector<Modint<MOD>>{};
    sparse_formal_power_series_internal::check_characteristic<MOD>(
        size, "sparse formal power series square root requires target size at most MOD"
    );
    const auto normalized =
        sparse_formal_power_series_internal::normalized_prefix<MOD>(terms, size);
    if(normalized.empty()) return std::vector<Modint<MOD>>(size);
    if constexpr(MOD == 2){
        throw std::runtime_error(
            "sparse formal power series square root requires odd characteristic"
        );
    }

    const std::size_t first_degree = normalized.front().first;
    if((first_degree & 1U) != 0) return std::nullopt;
    const auto leading_root = sparse_formal_power_series_internal::modular_sqrt<MOD>(
        normalized.front().second
    );
    if(!leading_root) return std::nullopt;

    const std::size_t shift = first_degree / 2;
    const std::size_t root_size = size - shift;
    std::vector<SparseFormalPowerSeriesTerm<MOD>> unit;
    unit.reserve(normalized.size());
    for(const auto& [degree, coefficient]: normalized){
        const std::size_t shifted_degree = degree - first_degree;
        if(shifted_degree >= root_size) break;
        unit.emplace_back(shifted_degree, coefficient);
    }

    const auto inverse_degree =
        sparse_formal_power_series_internal::inverse_degrees<MOD>(root_size);
    std::vector<Modint<MOD>> root(root_size);
    root[0] = *leading_root;
    const Modint<MOD> unit_constant = unit.front().second;
    const Modint<MOD> inverse_double_constant =
        (Modint<MOD>(2) * unit_constant).inv();
    for(std::size_t degree = 1; degree < root_size; ++degree){
        Modint<MOD> numerator = 0;
        for(auto iterator = unit.begin() + 1; iterator != unit.end(); ++iterator){
            const std::size_t term_degree = iterator->first;
            if(term_degree > degree) break;
            const Modint<MOD> multiplier =
                Modint<MOD>(3) *
                    sparse_formal_power_series_internal::from_size<MOD>(term_degree) -
                Modint<MOD>(2) *
                    sparse_formal_power_series_internal::from_size<MOD>(degree);
            numerator += multiplier * iterator->second * root[degree - term_degree];
        }
        root[degree] = numerator * inverse_degree[degree] * inverse_double_constant;
    }

    std::vector<Modint<MOD>> result(size);
    for(std::size_t degree = 0; degree < root_size; ++degree){
        result[shift + degree] = root[degree];
    }
    return result;
}
