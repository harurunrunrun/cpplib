#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "multipoint_evaluation.hpp"
#include "polynomial_interpolation.hpp"

namespace math{

inline constexpr std::uint64_t associative_sieve_max_n = 10'000'000'000ULL;
inline constexpr std::uint32_t associative_sieve_max_exponent = 1000;

using AssociativeSieveOperation =
    std::array<std::array<std::uint8_t, 4>, 4>;
using AssociativeSieveValue = std::array<Modint998244353, 4>;

namespace associative_sieve_internal{

using mint = Modint998244353;
using Value = AssociativeSieveValue;

inline std::uint64_t integer_square_root(std::uint64_t value){
    std::uint64_t low = 0;
    std::uint64_t high = 100'001;
    while(low + 1 < high){
        const std::uint64_t middle = low + (high - low) / 2;
        if(middle <= value / middle) low = middle;
        else high = middle;
    }
    return low;
}

class QuotientTable{
    std::uint64_t n_;
    std::uint64_t square_root_;
    std::size_t high_length_;
    std::size_t table_size_;
    std::vector<std::uint64_t> values_;

public:
    explicit QuotientTable(std::uint64_t n):
        n_(n),
        square_root_(integer_square_root(n)),
        high_length_(static_cast<std::size_t>(n / square_root_))
    {
        if(high_length_ != 1 &&
            n_ / (high_length_ - 1) == square_root_){
            --high_length_;
        }
        table_size_ = high_length_ + static_cast<std::size_t>(square_root_);
        values_.assign(table_size_, 0);
        for(std::size_t index = 1; index < high_length_; ++index){
            values_[index] = n_ / index;
        }
        for(std::uint64_t value = square_root_ + 1; value-- > 1;){
            values_[table_size_ - static_cast<std::size_t>(value)] = value;
        }
    }

    std::uint64_t square_root() const{ return square_root_; }
    std::size_t size() const{ return table_size_; }
    std::uint64_t value(std::size_t index) const{ return values_[index]; }

    std::size_t index_of(std::uint64_t value) const{
        if(value == 0 || value > n_)[[unlikely]]{
            throw std::logic_error("invalid associative-sieve quotient lookup");
        }
        if(value <= square_root_){
            return table_size_ - static_cast<std::size_t>(value);
        }
        return static_cast<std::size_t>(n_ / value);
    }
};

inline std::vector<std::uint32_t> enumerate_primes(std::uint64_t limit){
    std::vector<bool> composite(static_cast<std::size_t>(limit + 1), false);
    std::vector<std::uint32_t> primes;
    for(std::uint64_t value = 2; value <= limit; ++value){
        if(composite[static_cast<std::size_t>(value)]) continue;
        primes.push_back(static_cast<std::uint32_t>(value));
        if(value <= limit / value){
            for(std::uint64_t multiple = value * value;
                multiple <= limit;
                multiple += value){
                composite[static_cast<std::size_t>(multiple)] = true;
            }
        }
    }
    return primes;
}

inline void validate_operation(const AssociativeSieveOperation& operation){
    for(std::size_t left = 0; left < 4; ++left){
        for(std::size_t right = 0; right < 4; ++right){
            if(operation[left][right] >= 4)[[unlikely]]{
                throw std::invalid_argument("associative-sieve operation is out of range");
            }
            if(operation[left][right] != operation[right][left])[[unlikely]]{
                throw std::invalid_argument("associative-sieve operation is not commutative");
            }
        }
        if(operation[0][left] != left)[[unlikely]]{
            throw std::invalid_argument("zero is not the associative-sieve identity");
        }
    }
    for(std::size_t first = 0; first < 4; ++first){
        for(std::size_t second = 0; second < 4; ++second){
            for(std::size_t third = 0; third < 4; ++third){
                if(operation[operation[first][second]][third] !=
                    operation[first][operation[second][third]])[[unlikely]]{
                    throw std::invalid_argument("associative-sieve operation is not associative");
                }
            }
        }
    }
}

inline Value add(Value left, const Value& right){
    for(std::size_t state = 0; state < 4; ++state) left[state] += right[state];
    return left;
}

inline Value subtract(Value left, const Value& right){
    for(std::size_t state = 0; state < 4; ++state) left[state] -= right[state];
    return left;
}

inline Value multiply(
    const Value& left,
    const Value& right,
    const AssociativeSieveOperation& operation
){
    Value result{};
    for(std::size_t first = 0; first < 4; ++first){
        for(std::size_t second = 0; second < 4; ++second){
            result[operation[first][second]] += left[first] * right[second];
        }
    }
    return result;
}

inline Value prime_power_value(
    std::uint64_t prime,
    std::uint32_t exponent,
    mint prime_weight
){
    Value result{};
    const std::size_t state = static_cast<std::size_t>(
        prime % 4 * (exponent % 4) % 4
    );
    result[state] = prime_weight.pow(exponent);
    return result;
}

inline Polynomial<998244353> residue_power_prefix_polynomial(
    std::uint32_t exponent,
    std::uint32_t residue
){
    constexpr std::size_t capacity = associative_sieve_max_exponent + 2;
    using Interpolation = PolynomialInterpolation<mint, capacity>;
    typename Interpolation::ValueArray points{};
    typename Interpolation::ValueArray values{};
    const std::size_t sample_count = static_cast<std::size_t>(exponent) + 2;
    for(std::size_t count = 0; count < sample_count; ++count){
        points[count] = mint(static_cast<long long>(count));
        if(count == 0) continue;
        const std::uint64_t term =
            4 * static_cast<std::uint64_t>(count - 1) + residue;
        values[count] = values[count - 1] +
            mint(static_cast<long long>(term)).pow(exponent);
    }
    const Interpolation interpolation;
    const auto coefficients = interpolation.interpolate_coefficients(
        points, values, sample_count
    );
    return Polynomial<998244353>(
        coefficients.begin(),
        coefficients.begin() + static_cast<std::ptrdiff_t>(sample_count)
    );
}

inline std::vector<mint> residue_power_prefixes(
    const QuotientTable& table,
    std::uint32_t exponent,
    std::uint32_t residue
){
    std::vector<mint> points;
    points.reserve(table.size() - 1);
    for(std::size_t index = 1; index < table.size(); ++index){
        const std::uint64_t value = table.value(index);
        const std::uint64_t count =
            value < residue ? 0 : (value - residue) / 4 + 1;
        points.emplace_back(static_cast<long long>(count));
    }
    return multipoint_evaluation<998244353>(
        residue_power_prefix_polynomial(exponent, residue), points
    );
}

inline std::vector<Value> prime_prefix_table(
    const QuotientTable& table,
    const std::vector<std::uint32_t>& primes,
    const std::vector<mint>& prime_weights,
    std::uint32_t exponent
){
    std::vector<Value> result(table.size());
    const std::vector<mint> residue_one =
        residue_power_prefixes(table, exponent, 1);
    const std::vector<mint> residue_three =
        residue_power_prefixes(table, exponent, 3);
    for(std::size_t index = 1; index < table.size(); ++index){
        result[index][1] = residue_one[index - 1] - mint(1);
        result[index][3] = residue_three[index - 1];
    }

    Value primes_before{};
    for(std::size_t prime_index = 1;
        prime_index < primes.size();
        ++prime_index){
        const std::uint64_t prime = primes[prime_index];
        const std::uint64_t square = prime * prime;
        const mint weight = prime_weights[prime_index];
        for(std::size_t index = 1;
            index < table.size() && table.value(index) >= square;
            ++index){
            const std::size_t source_index = table.index_of(
                table.value(index) / prime
            );
            for(std::size_t source_state: {std::size_t(1), std::size_t(3)}){
                const std::size_t target_state = static_cast<std::size_t>(
                    prime % 4 * source_state % 4
                );
                result[index][target_state] -= weight * (
                    result[source_index][source_state] -
                    primes_before[source_state]
                );
            }
        }
        primes_before[static_cast<std::size_t>(prime % 4)] += weight;
    }

    const mint two_weight = mint(2).pow(exponent);
    for(std::size_t index = 1; index < table.size(); ++index){
        if(table.value(index) >= 2) result[index][2] = two_weight;
    }
    return result;
}

} // namespace associative_sieve_internal

inline std::vector<AssociativeSieveValue> associative_sieve_summatory_quotients(
    std::uint64_t n,
    std::uint32_t exponent,
    const AssociativeSieveOperation& operation
){
    using namespace associative_sieve_internal;
    if(n == 0 || n > associative_sieve_max_n)[[unlikely]]{
        throw std::invalid_argument("associative sieve requires 1 <= n <= 10^10");
    }
    if(exponent > associative_sieve_max_exponent)[[unlikely]]{
        throw std::invalid_argument("associative sieve requires exponent <= 1000");
    }
    validate_operation(operation);

    const QuotientTable table(n);
    const std::vector<std::uint32_t> primes =
        enumerate_primes(table.square_root());
    std::vector<mint> prime_weights;
    prime_weights.reserve(primes.size());
    for(const std::uint32_t prime: primes){
        prime_weights.push_back(mint(prime).pow(exponent));
    }
    const std::vector<Value> prime_prefix =
        prime_prefix_table(table, primes, prime_weights, exponent);
    std::vector<Value> result = prime_prefix;

    for(std::size_t reversed = primes.size(); reversed > 0; --reversed){
        const std::size_t prime_index = reversed - 1;
        const std::uint64_t prime = primes[prime_index];
        const std::uint64_t square = prime * prime;
        const mint prime_weight = prime_weights[prime_index];
        const Value primes_through_current =
            prime_prefix[table.index_of(prime)];
        for(std::size_t index = 1;
            index < table.size() && table.value(index) >= square;
            ++index){
            const std::uint64_t limit = table.value(index);
            std::uint64_t power = prime;
            std::uint32_t power_exponent = 1;
            while(power <= limit / prime){
                const Value remaining = subtract(
                    result[table.index_of(limit / power)],
                    primes_through_current
                );
                result[index] = add(
                    result[index],
                    multiply(
                        prime_power_value(
                            prime, power_exponent, prime_weight
                        ),
                        remaining,
                        operation
                    )
                );
                result[index] = add(
                    result[index],
                    prime_power_value(
                        prime, power_exponent + 1, prime_weight
                    )
                );
                power *= prime;
                ++power_exponent;
            }
        }
    }

    std::vector<AssociativeSieveValue> answers;
    answers.reserve(static_cast<std::size_t>(table.square_root()));
    for(std::size_t index = 1;
        index <= static_cast<std::size_t>(table.square_root());
        ++index){
        result[index][0] += mint(1);
        answers.push_back(result[index]);
    }
    return answers;
}

} // namespace math
