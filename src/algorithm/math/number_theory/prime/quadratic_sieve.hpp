#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_QUADRATIC_SIEVE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_QUADRATIC_SIEVE_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../modular/sqrt_mod.hpp"
#include "prime_factorization.hpp"

namespace math {

struct QuadraticSieveResult {
    std::optional<u64> factor;
    std::size_t tested_values = 0;
    std::size_t smooth_relations = 0;
    std::size_t factor_base_size = 0;
};

namespace quadratic_sieve_internal {

inline u64 ceiling_square_root(u64 value) {
    u64 root = static_cast<u64>(
        std::sqrt(static_cast<long double>(value))
    );
    while(root != 0 && root > value / root) --root;
    while(root < value / root
          || (root <= value / root && root * root < value)){
        ++root;
    }
    return root;
}

inline std::vector<std::uint32_t> primes_up_to(std::uint32_t limit) {
    std::vector<unsigned char> composite(
        static_cast<std::size_t>(limit) + 1, 0
    );
    std::vector<std::uint32_t> primes;
    for(std::uint32_t value = 2; value <= limit; ++value){
        if(composite[value]) continue;
        primes.push_back(value);
        if(static_cast<std::uint64_t>(value) * value > limit) continue;
        for(std::uint32_t multiple = value * value;
            multiple <= limit;
            multiple += value){
            composite[multiple] = 1;
        }
    }
    return primes;
}

inline bool bit(const std::vector<u64>& blocks, std::size_t index) {
    return (blocks[index >> 6] >> (index & 63U)) & 1U;
}

inline void xor_into(
    std::vector<u64>& target,
    const std::vector<u64>& source
) {
    for(std::size_t index = 0; index < target.size(); ++index){
        target[index] ^= source[index];
    }
}

inline bool all_zero(const std::vector<u64>& blocks) {
    for(const u64 value : blocks){
        if(value != 0) return false;
    }
    return true;
}

struct Relation {
    u64 x = 0;
    std::vector<std::uint32_t> exponents;
    std::vector<u64> parity;
};

inline std::optional<u64> dependency_factor(
    u64 value,
    const std::vector<std::uint32_t>& factor_base,
    const std::vector<Relation>& relations,
    const std::vector<u64>& dependency
) {
    u64 left = 1 % value;
    std::vector<std::uint64_t> exponent_sum(factor_base.size());
    for(std::size_t relation = 0;
        relation < relations.size();
        ++relation){
        if(!bit(dependency, relation)) continue;
        left = mul_mod_u64(left, relations[relation].x % value, value);
        for(std::size_t prime = 0; prime < factor_base.size(); ++prime){
            exponent_sum[prime] += relations[relation].exponents[prime];
        }
    }
    u64 right = 1 % value;
    for(std::size_t prime = 0; prime < factor_base.size(); ++prime){
        if(exponent_sum[prime] & 1U) return std::nullopt;
        right = mul_mod_u64(
            right,
            pow_mod_u64(
                factor_base[prime], exponent_sum[prime] / 2, value
            ),
            value
        );
    }
    const u64 difference = left >= right ? left - right : right - left;
    u64 divisor = std::gcd(difference, value);
    if(divisor != 1 && divisor != value) return divisor;
    const u64 sum = static_cast<u64>(
        (static_cast<unsigned __int128>(left) + right) % value
    );
    divisor = std::gcd(sum, value);
    if(divisor != 1 && divisor != value) return divisor;
    return std::nullopt;
}

}  // namespace quadratic_sieve_internal

inline QuadraticSieveResult quadratic_sieve_factor(
    u64 value,
    std::uint32_t factor_base_bound = 1000,
    std::size_t maximum_tested_values = 1'000'000,
    std::size_t extra_relations = 16
) {
    if(value < 2){
        throw std::invalid_argument(
            "quadratic_sieve_factor needs value >= 2"
        );
    }
    QuadraticSieveResult result;
    if(value % 2 == 0){
        result.factor = 2;
        return result;
    }
    if(is_prime_miller_rabin(value)){
        result.factor = value;
        return result;
    }
    if(factor_base_bound < 2 || maximum_tested_values == 0){
        throw std::invalid_argument(
            "quadratic sieve bounds must be positive"
        );
    }

    std::vector<std::uint32_t> factor_base;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> factor_roots;
    for(const std::uint32_t prime :
        quadratic_sieve_internal::primes_up_to(factor_base_bound)){
        if(value % prime == 0){
            result.factor = prime;
            result.factor_base_size = factor_base.size();
            return result;
        }
        if(prime == 2
           || pow_mod_u64(value % prime, (prime - 1) / 2, prime) == 1){
            factor_base.push_back(prime);
            if(prime == 2){
                factor_roots.emplace_back(1, 1);
            }else{
                const auto root = sqrt_mod(value % prime, prime);
                if(!root){
                    throw std::logic_error(
                        "quadratic sieve factor-base root is missing"
                    );
                }
                factor_roots.emplace_back(
                    static_cast<std::uint32_t>(*root),
                    static_cast<std::uint32_t>(prime - *root)
                );
            }
        }
    }
    result.factor_base_size = factor_base.size();
    if(factor_base.empty()) return result;

    const std::size_t parity_blocks =
        (factor_base.size() + 63) / 64;
    const std::size_t relation_limit =
        factor_base.size() + extra_relations;
    const std::size_t relation_blocks =
        (relation_limit + 63) / 64;
    std::vector<quadratic_sieve_internal::Relation> relations;
    relations.reserve(relation_limit);
    std::vector<std::vector<u64>> basis_value(
        factor_base.size(),
        std::vector<u64>(parity_blocks)
    );
    std::vector<std::vector<u64>> basis_combination(
        factor_base.size(),
        std::vector<u64>(relation_blocks)
    );
    std::vector<unsigned char> has_basis(factor_base.size(), 0);

    const u64 start = quadratic_sieve_internal::ceiling_square_root(value);
    constexpr std::size_t sieve_block_size = 1U << 16;
    std::size_t offset = 0;
    while(offset < maximum_tested_values
          && relations.size() < relation_limit){
        const std::size_t requested = std::min(
            sieve_block_size, maximum_tested_values - offset
        );
        std::vector<u64> residual;
        residual.reserve(requested);
        for(std::size_t local = 0; local < requested; ++local){
            if(offset + local
               > std::numeric_limits<u64>::max() - start){
                break;
            }
            const u64 x = start + static_cast<u64>(offset + local);
            const unsigned __int128 difference =
                static_cast<unsigned __int128>(x) * x - value;
            if(difference > std::numeric_limits<u64>::max()) break;
            const u64 current = static_cast<u64>(difference);
            if(current == 0){
                const u64 divisor = std::gcd(x, value);
                if(divisor != 1 && divisor != value){
                    result.factor = divisor;
                }
                result.tested_values += local + 1;
                return result;
            }
            residual.push_back(current);
        }
        if(residual.empty()) break;

        const u64 block_start = start + static_cast<u64>(offset);
        for(std::size_t prime_index = 0;
            prime_index < factor_base.size();
            ++prime_index){
            const std::uint32_t prime = factor_base[prime_index];
            const auto [first_root, second_root] =
                factor_roots[prime_index];
            const auto sieve_root = [&](std::uint32_t root){
                const std::uint32_t remainder =
                    static_cast<std::uint32_t>(block_start % prime);
                std::size_t local = static_cast<std::size_t>(
                    (root + prime - remainder) % prime
                );
                for(; local < residual.size(); local += prime){
                    while(residual[local] % prime == 0){
                        residual[local] /= prime;
                    }
                }
            };
            sieve_root(first_root);
            if(second_root != first_root) sieve_root(second_root);
        }
        result.tested_values += residual.size();

        for(std::size_t local = 0;
            local < residual.size()
                && relations.size() < relation_limit;
            ++local){
            if(residual[local] != 1) continue;
            const u64 x = block_start + static_cast<u64>(local);
            u64 remaining = static_cast<u64>(
                static_cast<unsigned __int128>(x) * x - value
            );
            quadratic_sieve_internal::Relation relation;
            relation.x = x % value;
            relation.exponents.assign(factor_base.size(), 0);
            relation.parity.assign(parity_blocks, 0);
            for(std::size_t index = 0;
                index < factor_base.size();
                ++index){
                const std::uint32_t prime = factor_base[index];
                while(remaining % prime == 0){
                    remaining /= prime;
                    ++relation.exponents[index];
                }
                if(relation.exponents[index] & 1U){
                    relation.parity[index >> 6] |=
                        u64{1} << (index & 63U);
                }
            }
            if(remaining != 1){
                throw std::logic_error(
                    "quadratic sieve smoothness mismatch"
                );
            }
            const std::size_t relation_index = relations.size();
            relations.push_back(std::move(relation));
            result.smooth_relations = relations.size();

            std::vector<u64> reduced = relations.back().parity;
            std::vector<u64> combination(relation_blocks);
            combination[relation_index >> 6] |=
                u64{1} << (relation_index & 63U);
            bool inserted = false;
            for(std::size_t reverse = 0;
                reverse < factor_base.size();
                ++reverse){
                const std::size_t pivot =
                    factor_base.size() - 1 - reverse;
                if(!quadratic_sieve_internal::bit(reduced, pivot)){
                    continue;
                }
                if(!has_basis[pivot]){
                    has_basis[pivot] = 1;
                    basis_value[pivot] = std::move(reduced);
                    basis_combination[pivot] = std::move(combination);
                    inserted = true;
                    break;
                }
                quadratic_sieve_internal::xor_into(
                    reduced, basis_value[pivot]
                );
                quadratic_sieve_internal::xor_into(
                    combination, basis_combination[pivot]
                );
            }
            if(inserted
               || !quadratic_sieve_internal::all_zero(reduced)){
                continue;
            }
            result.factor =
                quadratic_sieve_internal::dependency_factor(
                    value, factor_base, relations, combination
                );
            if(result.factor) return result;
        }
        offset += residual.size();
        if(residual.size() < requested) break;
    }
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_QUADRATIC_SIEVE_HPP_INCLUDED
