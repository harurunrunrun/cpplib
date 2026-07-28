#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_ANALYTIC_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_ANALYTIC_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace set_power_series_analytic_detail{

template<class T>
struct RankedSetPowerSeries{
    std::size_t bit_count = 0;
    std::size_t rank_count = 0;
    std::vector<T> coefficients;
};

template<class T>
RankedSetPowerSeries<T> ranked_zeta_transform(
    const std::vector<T>& series
){
    if(series.empty() || !std::has_single_bit(series.size()))[[unlikely]]{
        throw std::invalid_argument(
            "set power series size must be a nonempty power of two"
        );
    }
    const std::size_t bit_count = std::bit_width(series.size()) - 1;
    const std::size_t rank_count = bit_count + 1;
    if(rank_count > std::numeric_limits<std::size_t>::max()
        / series.size())[[unlikely]]{
        throw std::length_error("set power series is too large");
    }
    RankedSetPowerSeries<T> result{
        bit_count,
        rank_count,
        std::vector<T>(series.size() * rank_count),
    };
    for(std::size_t subset = 0; subset < series.size(); ++subset){
        result.coefficients[
            subset * rank_count + std::popcount(subset)
        ] = series[subset];
    }
    for(std::size_t bit = 0; bit < bit_count; ++bit){
        const std::size_t flag = std::size_t{1} << bit;
        for(std::size_t subset = 0; subset < series.size(); ++subset){
            if((subset & flag) == 0) continue;
            const std::size_t source = subset ^ flag;
            const std::size_t source_rank = std::popcount(source);
            const std::size_t target_offset = subset * rank_count;
            const std::size_t source_offset = source * rank_count;
            for(std::size_t rank = 0; rank <= source_rank; ++rank){
                result.coefficients[target_offset + rank] +=
                    result.coefficients[source_offset + rank];
            }
        }
    }
    return result;
}

template<class T>
std::vector<T> ranked_mobius_extract(
    RankedSetPowerSeries<T> transformed
){
    const std::size_t subset_count =
        transformed.coefficients.size() / transformed.rank_count;
    for(std::size_t bit = 0; bit < transformed.bit_count; ++bit){
        const std::size_t flag = std::size_t{1} << bit;
        for(std::size_t subset = 0; subset < subset_count; ++subset){
            if((subset & flag) == 0) continue;
            const std::size_t source = subset ^ flag;
            const std::size_t target_offset =
                subset * transformed.rank_count;
            const std::size_t source_offset =
                source * transformed.rank_count;
            for(std::size_t rank = 0; rank <= transformed.bit_count; ++rank){
                transformed.coefficients[target_offset + rank] -=
                    transformed.coefficients[source_offset + rank];
            }
        }
    }
    std::vector<T> result(subset_count);
    for(std::size_t subset = 0; subset < subset_count; ++subset){
        result[subset] = transformed.coefficients[
            subset * transformed.rank_count + std::popcount(subset)
        ];
    }
    return result;
}

}  // namespace set_power_series_analytic_detail

template<class T>
std::vector<T> set_power_series_exponential(const std::vector<T>& series){
    using namespace set_power_series_analytic_detail;
    if(series.empty() || !(series[0] == T{}))[[unlikely]]{
        throw std::domain_error(
            "set power series exponential requires zero empty coefficient"
        );
    }
    RankedSetPowerSeries<T> transformed = ranked_zeta_transform(series);
    std::vector<T> polynomial(transformed.rank_count);
    for(std::size_t subset = 0; subset < series.size(); ++subset){
        const std::size_t degree = transformed.bit_count;
        const std::size_t offset = subset * transformed.rank_count;
        for(std::size_t index = 0; index <= degree; ++index){
            polynomial[index] = transformed.coefficients[offset + index];
        }
        transformed.coefficients[offset] = T{1};
        for(std::size_t current = 1; current <= degree; ++current){
            T value{};
            for(std::size_t source = 1; source <= current; ++source){
                value += T{static_cast<long long>(source)}
                    * polynomial[source]
                    * transformed.coefficients[
                        offset + current - source
                    ];
            }
            transformed.coefficients[offset + current] =
                value / T{static_cast<long long>(current)};
        }
    }
    return ranked_mobius_extract(std::move(transformed));
}

template<class T>
std::vector<T> set_power_series_logarithm(const std::vector<T>& series){
    using namespace set_power_series_analytic_detail;
    if(series.empty() || !(series[0] == T{1}))[[unlikely]]{
        throw std::domain_error(
            "set power series logarithm requires unit empty coefficient"
        );
    }
    RankedSetPowerSeries<T> transformed = ranked_zeta_transform(series);
    std::vector<T> polynomial(transformed.rank_count);
    for(std::size_t subset = 0; subset < series.size(); ++subset){
        const std::size_t degree = transformed.bit_count;
        const std::size_t offset = subset * transformed.rank_count;
        for(std::size_t index = 0; index <= degree; ++index){
            polynomial[index] = transformed.coefficients[offset + index];
        }
        transformed.coefficients[offset] = T{};
        for(std::size_t current = 1; current <= degree; ++current){
            T value = T{static_cast<long long>(current)}
                * polynomial[current];
            for(std::size_t source = 1; source < current; ++source){
                value -= T{static_cast<long long>(source)}
                    * transformed.coefficients[offset + source]
                    * polynomial[current - source];
            }
            transformed.coefficients[offset + current] =
                value / T{static_cast<long long>(current)};
        }
    }
    return ranked_mobius_extract(std::move(transformed));
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_ANALYTIC_HPP_INCLUDED
