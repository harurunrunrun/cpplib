#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_CONSTANT_TIME_GCD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_CONSTANT_TIME_GCD_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class ConstantTimeGcd{
    using Value = std::uint32_t;

    Value maximum_ = 0;
    Value small_limit_ = 0;
    std::size_t table_width_ = 1;
    std::vector<std::array<Value, 3>> factors_;
    std::vector<Value> small_gcd_;

    static Value integer_square_root(Value value){
        Value low = 0;
        Value high = Value{1} << 16;
        while(low + 1 < high){
            const Value middle = low + (high - low) / 2;
            if(static_cast<std::uint64_t>(middle) * middle <= value){
                low = middle;
            }else{
                high = middle;
            }
        }
        return low;
    }

    Value small_gcd(Value first, Value second) const{
        return small_gcd_[
            static_cast<std::size_t>(first) * table_width_ + second
        ];
    }

public:
    explicit ConstantTimeGcd(Value maximum)
        : maximum_(maximum),
          small_limit_(integer_square_root(maximum)),
          table_width_(static_cast<std::size_t>(small_limit_) + 1),
          factors_(static_cast<std::size_t>(maximum) + 1, {1, 1, 1}),
          small_gcd_(table_width_ * table_width_, 0){
        for(Value first = 0; first <= small_limit_; ++first){
            small_gcd_[static_cast<std::size_t>(first) * table_width_] = first;
            for(Value second = 1; second <= first; ++second){
                const Value result = small_gcd(
                    second, static_cast<Value>(first % second)
                );
                small_gcd_[
                    static_cast<std::size_t>(first) * table_width_ + second
                ] = result;
                small_gcd_[
                    static_cast<std::size_t>(second) * table_width_ + first
                ] = result;
            }
        }

        if(maximum < 2) return;
        std::vector<Value> smallest_prime(
            static_cast<std::size_t>(maximum) + 1, 0
        );
        std::vector<Value> primes;
        for(std::uint64_t raw_value = 2; raw_value <= maximum; ++raw_value){
            const Value value = static_cast<Value>(raw_value);
            if(smallest_prime[value] == 0){
                smallest_prime[value] = value;
                primes.push_back(value);
            }
            for(const Value prime: primes){
                const std::uint64_t product =
                    static_cast<std::uint64_t>(value) * prime;
                if(product > maximum || prime > smallest_prime[value]) break;
                smallest_prime[static_cast<std::size_t>(product)] = prime;
            }

            const Value prime = smallest_prime[value];
            std::array<Value, 3> decomposition = factors_[value / prime];
            decomposition[0] = static_cast<Value>(decomposition[0] * prime);
            if(decomposition[1] < decomposition[0]){
                std::swap(decomposition[0], decomposition[1]);
            }
            if(decomposition[2] < decomposition[1]){
                std::swap(decomposition[1], decomposition[2]);
            }
            factors_[value] = decomposition;
        }
    }

    Value max_value() const{
        return maximum_;
    }

    Value query(Value first, Value second) const{
        if(first > maximum_ || second > maximum_){
            throw std::out_of_range("ConstantTimeGcd query exceeds max_value");
        }
        if(first == 0) return second;
        if(second == 0) return first;

        Value result = 1;
        Value remaining = second;
        for(const Value factor: factors_[first]){
            Value common;
            if(factor <= small_limit_){
                common = small_gcd(
                    factor, static_cast<Value>(remaining % factor)
                );
            }else{
                common = remaining % factor == 0 ? factor : 1;
            }
            result = static_cast<Value>(result * common);
            remaining = static_cast<Value>(remaining / common);
        }
        return result;
    }

    Value operator()(Value first, Value second) const{
        return query(first, second);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_CONSTANT_TIME_GCD_HPP_INCLUDED
