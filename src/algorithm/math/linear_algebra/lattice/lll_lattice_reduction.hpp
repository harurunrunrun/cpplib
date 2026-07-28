#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_LATTICE_LLL_LATTICE_REDUCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_LATTICE_LLL_LATTICE_REDUCTION_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace linear_algebra {

struct LllLatticeReductionResult {
    std::vector<std::vector<long long>> basis;
    std::vector<std::vector<long long>> transformation;
    std::size_t swaps = 0;
};

namespace lll_lattice_reduction_internal {

inline long long checked_subtract_multiple(
    long long value,
    long long multiplier,
    long long source
) {
    const __int128 result =
        static_cast<__int128>(value)
        - static_cast<__int128>(multiplier)
            * static_cast<__int128>(source);
    if(result < std::numeric_limits<long long>::min()
       || result > std::numeric_limits<long long>::max()){
        throw std::overflow_error("LLL integer operation overflowed");
    }
    return static_cast<long long>(result);
}

struct GramSchmidt {
    std::vector<std::vector<long double>> orthogonal;
    std::vector<std::vector<long double>> mu;
    std::vector<long double> squared_norm;
};

inline GramSchmidt gram_schmidt(
    const std::vector<std::vector<long long>>& basis
) {
    const std::size_t count = basis.size();
    const std::size_t dimension = basis.empty() ? 0 : basis.front().size();
    GramSchmidt result;
    result.orthogonal.assign(
        count, std::vector<long double>(dimension)
    );
    result.mu.assign(count, std::vector<long double>(count));
    result.squared_norm.assign(count, 0.0L);
    for(std::size_t vector_index = 0;
        vector_index < count;
        ++vector_index){
        for(std::size_t coordinate = 0;
            coordinate < dimension;
            ++coordinate){
            result.orthogonal[vector_index][coordinate] =
                static_cast<long double>(
                    basis[vector_index][coordinate]
                );
        }
        for(std::size_t previous = 0;
            previous < vector_index;
            ++previous){
            long double dot = 0.0L;
            for(std::size_t coordinate = 0;
                coordinate < dimension;
                ++coordinate){
                dot += static_cast<long double>(
                           basis[vector_index][coordinate]
                       )
                    * result.orthogonal[previous][coordinate];
            }
            result.mu[vector_index][previous] =
                dot / result.squared_norm[previous];
            for(std::size_t coordinate = 0;
                coordinate < dimension;
                ++coordinate){
                result.orthogonal[vector_index][coordinate] -=
                    result.mu[vector_index][previous]
                    * result.orthogonal[previous][coordinate];
            }
        }
        for(const long double value : result.orthogonal[vector_index]){
            result.squared_norm[vector_index] += value * value;
        }
        if(!(result.squared_norm[vector_index] > 0.0L)
           || !std::isfinite(result.squared_norm[vector_index])){
            throw std::invalid_argument(
                "LLL basis must be finite and linearly independent"
            );
        }
    }
    return result;
}

inline void subtract_multiple(
    std::vector<long long>& target,
    const std::vector<long long>& source,
    long long multiplier
) {
    for(std::size_t index = 0; index < target.size(); ++index){
        target[index] = checked_subtract_multiple(
            target[index], multiplier, source[index]
        );
    }
}

}  // namespace lll_lattice_reduction_internal

inline LllLatticeReductionResult lll_lattice_reduction(
    std::vector<std::vector<long long>> basis,
    long double delta = 0.75L
) {
    if(!(0.25L < delta && delta < 1.0L) || !std::isfinite(delta)){
        throw std::invalid_argument("LLL delta must be in (1/4,1)");
    }
    const std::size_t count = basis.size();
    const std::size_t dimension =
        basis.empty() ? 0 : basis.front().size();
    for(const auto& vector : basis){
        if(vector.size() != dimension){
            throw std::invalid_argument(
                "LLL basis vectors have different dimensions"
            );
        }
    }
    LllLatticeReductionResult result;
    result.basis = std::move(basis);
    result.transformation.assign(
        count, std::vector<long long>(count)
    );
    for(std::size_t index = 0; index < count; ++index){
        result.transformation[index][index] = 1;
    }
    if(count <= 1) return result;

    auto gram =
        lll_lattice_reduction_internal::gram_schmidt(result.basis);
    std::size_t current = 1;
    while(current < count){
        for(std::size_t offset = 0; offset < current; ++offset){
            const std::size_t previous = current - 1 - offset;
            const long double rounded =
                std::round(gram.mu[current][previous]);
            if(rounded < static_cast<long double>(
                    std::numeric_limits<long long>::min()
                )
               || rounded > static_cast<long double>(
                    std::numeric_limits<long long>::max()
                )){
                throw std::overflow_error(
                    "LLL size-reduction coefficient overflowed"
                );
            }
            const long long multiplier =
                static_cast<long long>(rounded);
            if(multiplier == 0) continue;
            lll_lattice_reduction_internal::subtract_multiple(
                result.basis[current],
                result.basis[previous],
                multiplier
            );
            lll_lattice_reduction_internal::subtract_multiple(
                result.transformation[current],
                result.transformation[previous],
                multiplier
            );
            gram = lll_lattice_reduction_internal::gram_schmidt(
                result.basis
            );
        }
        const long double lovasz_right =
            (delta - gram.mu[current][current - 1]
                         * gram.mu[current][current - 1])
            * gram.squared_norm[current - 1];
        if(gram.squared_norm[current] >= lovasz_right){
            ++current;
            continue;
        }
        std::swap(result.basis[current], result.basis[current - 1]);
        std::swap(
            result.transformation[current],
            result.transformation[current - 1]
        );
        ++result.swaps;
        gram = lll_lattice_reduction_internal::gram_schmidt(result.basis);
        if(current > 1) --current;
    }
    return result;
}

}  // namespace linear_algebra

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_LATTICE_LLL_LATTICE_REDUCTION_HPP_INCLUDED
