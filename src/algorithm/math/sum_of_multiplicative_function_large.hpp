#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/modint/modint.hpp"

namespace math{

inline constexpr std::uint64_t sum_multiplicative_function_large_max_n =
    10'000'000'000'000ULL;

template<int MOD>
struct LargePrimePrefixComponents{
    using mint = Modint<MOD>;

    std::vector<mint> prime_count;
    std::vector<mint> prime_sum;
};

template<int MOD>
struct LinearPrimePowerMultiplicativeFunction{
    using mint = Modint<MOD>;

    mint exponent_coefficient;
    mint prime_coefficient;

    mint operator()(std::uint64_t prime, std::uint32_t exponent) const{
        return exponent_coefficient * mint(static_cast<long long>(exponent)) +
            prime_coefficient *
            mint(static_cast<long long>(prime % static_cast<std::uint64_t>(MOD)));
    }
};

namespace sum_multiplicative_function_large_internal{

inline std::uint64_t integer_square_root(std::uint64_t value){
    if(value == 0) return 0;
    std::uint64_t low = 1;
    std::uint64_t high = std::min<std::uint64_t>(value, 4'000'000) + 1;
    while(low + 1 < high){
        const std::uint64_t middle = low + (high - low) / 2;
        if(middle <= value / middle) low = middle;
        else high = middle;
    }
    return low;
}

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

} // namespace sum_multiplicative_function_large_internal

template<int MOD>
class LargeMultiplicativeFunctionSummatory{
public:
    using mint = Modint<MOD>;
    using Components = LargePrimePrefixComponents<MOD>;

private:
    struct PrimePrefixPair{
        mint count;
        mint sum;
    };

    std::uint64_t n_ = 0;
    std::uint64_t square_root_ = 0;
    std::size_t high_length_ = 0;
    std::size_t table_size_ = 0;
    std::vector<std::uint32_t> primes_;

    static mint triangular(std::uint64_t value){
        std::uint64_t left = value;
        std::uint64_t right = value + 1;
        if((left & 1U) == 0) left /= 2;
        else right /= 2;
        return mint(static_cast<long long>(left % MOD)) *
            mint(static_cast<long long>(right % MOD));
    }

    std::size_t index_of(std::uint64_t value) const{
        if(value == 0 || value > n_)[[unlikely]]{
            throw std::logic_error("invalid quotient-table lookup");
        }
        if(value <= square_root_){
            return table_size_ - static_cast<std::size_t>(value);
        }
        return static_cast<std::size_t>(n_ / value);
    }

    template<class PrimePowerFunction>
    class Runner{
        const LargeMultiplicativeFunctionSummatory& owner_;
        const std::vector<mint>& prime_prefix_;
        PrimePowerFunction function_;
        mint answer_;

        mint prime_power(std::uint64_t prime, std::uint32_t exponent){
            return function_(prime, exponent);
        }

        void dfs(
            std::size_t prime_index,
            std::uint32_t exponent,
            std::uint64_t product,
            mint current
        ){
            const std::uint64_t prime = owner_.primes_[prime_index];
            answer_ += current * prime_power(prime, exponent + 1);

            const std::uint64_t limit = owner_.n_ / product;
            const std::uint64_t prime_square = prime * prime;
            if(prime_square <= limit){
                dfs(
                    prime_index,
                    exponent + 1,
                    product * prime,
                    current
                );
            }

            current *= prime_power(prime, exponent);
            answer_ += current * (
                prime_prefix_[owner_.index_of(limit)] -
                prime_prefix_[owner_.index_of(prime)]
            );

            for(std::size_t next = prime_index + 1;
                next < owner_.primes_.size();
                ++next){
                const std::uint64_t next_prime = owner_.primes_[next];
                const std::uint64_t next_square = next_prime * next_prime;
                if(next_square > limit) break;
                if(next_prime <= limit / next_square){
                    dfs(next, 1, product * next_prime, current);
                }else{
                    mint contribution = prime_power(next_prime, 2);
                    contribution += prime_power(next_prime, 1) * (
                        prime_prefix_[owner_.index_of(limit / next_prime)] -
                        prime_prefix_[owner_.index_of(next_prime)]
                    );
                    answer_ += current * contribution;
                }
            }
        }

    public:
        Runner(
            const LargeMultiplicativeFunctionSummatory& owner,
            const std::vector<mint>& prime_prefix,
            PrimePowerFunction function
        ):
            owner_(owner),
            prime_prefix_(prime_prefix),
            function_(std::move(function))
        {}

        mint run(){
            if(owner_.n_ == 0) return mint(0);
            answer_ = prime_prefix_[owner_.index_of(owner_.n_)] + mint(1);
            for(std::size_t index = 0; index < owner_.primes_.size(); ++index){
                dfs(index, 1, owner_.primes_[index], mint(1));
            }
            return answer_;
        }
    };

public:
    explicit LargeMultiplicativeFunctionSummatory(std::uint64_t n):
        n_(n)
    {
        static_assert(MOD >= 1);
        if(n_ > sum_multiplicative_function_large_max_n)[[unlikely]]{
            throw std::invalid_argument(
                "LargeMultiplicativeFunctionSummatory supports n <= 10^13"
            );
        }
        if(n_ == 0) return;

        square_root_ =
            sum_multiplicative_function_large_internal::integer_square_root(n_);
        high_length_ = static_cast<std::size_t>(n_ / square_root_);
        if(high_length_ != 1 &&
            n_ / (high_length_ - 1) == square_root_){
            --high_length_;
        }
        table_size_ = high_length_ + static_cast<std::size_t>(square_root_);
        primes_ =
            sum_multiplicative_function_large_internal::enumerate_primes(
                square_root_
            );
    }

    std::uint64_t limit() const{ return n_; }
    std::size_t quotient_table_size() const{ return table_size_; }

    std::vector<std::uint64_t> quotient_values() const{
        std::vector<std::uint64_t> result(table_size_);
        for(std::size_t index = 1; index < high_length_; ++index){
            result[index] = n_ / index;
        }
        for(std::uint64_t value = square_root_ + 1; value-- > 1;){
            result[table_size_ - static_cast<std::size_t>(value)] = value;
        }
        return result;
    }

    Components prime_prefix_components() const{
        if(n_ == 0) return {};

        std::vector<PrimePrefixPair> high(high_length_);
        for(std::size_t index = 1; index < high_length_; ++index){
            const std::uint64_t value = n_ / index;
            high[index].count = mint(static_cast<long long>((value - 1) % MOD));
            high[index].sum = triangular(value) - mint(1);
        }

        std::vector<PrimePrefixPair> low(
            static_cast<std::size_t>(square_root_ + 1)
        );
        for(std::uint64_t value = 1; value <= square_root_; ++value){
            low[static_cast<std::size_t>(value)].count =
                mint(static_cast<long long>((value - 1) % MOD));
            low[static_cast<std::size_t>(value)].sum =
                triangular(value) - mint(1);
        }

        for(std::size_t prime_index = 0;
            prime_index < primes_.size();
            ++prime_index){
            const std::uint64_t prime = primes_[prime_index];
            const std::uint64_t square = prime * prime;
            const mint primes_before_count(
                static_cast<long long>(prime_index % MOD)
            );
            const mint primes_before_sum =
                low[static_cast<std::size_t>(prime - 1)].sum;
            const mint prime_value(static_cast<long long>(prime % MOD));

            const std::size_t high_end = std::min<std::size_t>(
                high_length_,
                static_cast<std::size_t>(n_ / square + 1)
            );
            std::uint64_t multiplied_index = prime;
            for(std::size_t index = 1;
                index < high_end;
                ++index, multiplied_index += prime){
                const PrimePrefixPair& source =
                    multiplied_index < high_length_
                    ? high[static_cast<std::size_t>(multiplied_index)]
                    : low[static_cast<std::size_t>(n_ / multiplied_index)];
                high[index].count -= source.count - primes_before_count;
                high[index].sum -=
                    (source.sum - primes_before_sum) * prime_value;
            }

            for(std::uint64_t value = square_root_ + 1;
                value-- > square;){
                const PrimePrefixPair& source =
                    low[static_cast<std::size_t>(value / prime)];
                low[static_cast<std::size_t>(value)].count -=
                    source.count - primes_before_count;
                low[static_cast<std::size_t>(value)].sum -=
                    (source.sum - primes_before_sum) * prime_value;
            }
        }

        Components result;
        result.prime_count.reserve(table_size_);
        result.prime_sum.reserve(table_size_);
        for(const PrimePrefixPair& value: high){
            result.prime_count.push_back(value.count);
            result.prime_sum.push_back(value.sum);
        }
        for(std::uint64_t value = square_root_ + 1; value-- > 1;){
            const PrimePrefixPair& pair =
                low[static_cast<std::size_t>(value)];
            result.prime_count.push_back(pair.count);
            result.prime_sum.push_back(pair.sum);
        }
        return result;
    }

    template<class PrimePowerFunction>
    mint sum(
        PrimePowerFunction function,
        const std::vector<mint>& prime_prefix
    ) const{
        if(n_ == 0){
            if(!prime_prefix.empty())[[unlikely]]{
                throw std::invalid_argument(
                    "prime prefix table must be empty for n = 0"
                );
            }
            return mint(0);
        }
        if(prime_prefix.size() != table_size_)[[unlikely]]{
            throw std::invalid_argument("prime prefix table size mismatch");
        }
        return Runner<PrimePowerFunction>(
            *this, prime_prefix, std::move(function)
        ).run();
    }
};

template<int MOD = 469762049>
Modint<MOD> sum_of_multiplicative_function_large(
    std::uint64_t n,
    Modint<MOD> exponent_coefficient,
    Modint<MOD> prime_coefficient
){
    LargeMultiplicativeFunctionSummatory<MOD> summatory(n);
    if(n == 0) return Modint<MOD>(0);

    auto components = summatory.prime_prefix_components();
    for(std::size_t index = 0;
        index < components.prime_count.size();
        ++index){
        components.prime_count[index] =
            components.prime_count[index] * exponent_coefficient +
            components.prime_sum[index] * prime_coefficient;
    }
    std::vector<Modint<MOD>>().swap(components.prime_sum);

    const LinearPrimePowerMultiplicativeFunction<MOD> function{
        exponent_coefficient,
        prime_coefficient
    };
    return summatory.sum(function, components.prime_count);
}

} // namespace math
