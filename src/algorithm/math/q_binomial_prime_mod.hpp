#ifndef CPPLIB_SRC_ALGORITHM_MATH_Q_BINOMIAL_PRIME_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_Q_BINOMIAL_PRIME_MOD_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class QBinomialPrimeMod{
private:
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    u32 modulus_;
    u32 base_;
    std::size_t maximum_n_;
    std::size_t period_ = 0;
    std::vector<u32> q_factorial_;
    std::vector<u32> q_inverse_factorial_;
    std::vector<u32> factorial_;
    std::vector<u32> inverse_factorial_;

    static bool is_prime(u32 value){
        if(value < 2) return false;
        if(value % 2 == 0) return value == 2;
        for(u32 divisor = 3;
            static_cast<u64>(divisor) * divisor <= value;
            divisor += 2){
            if(value % divisor == 0) return false;
        }
        return true;
    }

    u32 add(u32 lhs, u32 rhs) const{
        const u64 sum = static_cast<u64>(lhs) + rhs;
        return static_cast<u32>(
            sum >= modulus_ ? sum - modulus_ : sum
        );
    }

    u32 subtract_one(u32 value) const{
        return value == 0 ? modulus_ - 1 : value - 1;
    }

    u32 multiply(u32 lhs, u32 rhs) const{
        return static_cast<u32>(static_cast<u64>(lhs) * rhs % modulus_);
    }

    u32 power(u32 value, u32 exponent) const{
        u32 result = 1 % modulus_;
        while(exponent != 0){
            if((exponent & 1U) != 0) result = multiply(result, value);
            value = multiply(value, value);
            exponent >>= 1;
        }
        return result;
    }

    u32 q_binomial_small(std::size_t n, std::size_t k) const{
        if(k > n) return 0;
        return multiply(q_factorial_[n],
                        multiply(q_inverse_factorial_[k],
                                 q_inverse_factorial_[n - k]));
    }

    u32 ordinary_binomial(std::size_t n, std::size_t k) const{
        if(k > n) return 0;
        return multiply(factorial_[n],
                        multiply(inverse_factorial_[k],
                                 inverse_factorial_[n - k]));
    }

    void build_q_factorials(){
        q_factorial_.reserve(maximum_n_ + 1);
        q_factorial_.push_back(1 % modulus_);

        u32 q_integer = 1 % modulus_;
        for(std::size_t index = 1; index <= maximum_n_; ++index){
            if(q_integer == 0){
                period_ = index;
                break;
            }
            q_factorial_.push_back(multiply(q_factorial_.back(), q_integer));
            q_integer = add(multiply(base_, q_integer), 1 % modulus_);
        }

        q_inverse_factorial_.resize(q_factorial_.size());
        q_inverse_factorial_.back() =
            power(q_factorial_.back(), modulus_ - 2);
        if(q_factorial_.size() == 1) return;

        if(base_ == 0){
            std::fill(q_inverse_factorial_.begin(),
                      q_inverse_factorial_.end(), 1 % modulus_);
            return;
        }

        const u32 inverse_q = power(base_, modulus_ - 2);
        u32 current_q_integer = period_ == 0
            ? multiply(subtract_one(q_integer), inverse_q)
            : multiply(modulus_ - 1, inverse_q);
        for(std::size_t index = q_factorial_.size() - 1;
            index > 0; --index){
            q_inverse_factorial_[index - 1] =
                multiply(q_inverse_factorial_[index], current_q_integer);
            current_q_integer =
                multiply(subtract_one(current_q_integer), inverse_q);
        }
    }

    void build_ordinary_factorials(){
        if(period_ == 0) return;
        const std::size_t maximum_high_digit = maximum_n_ / period_;
        factorial_.resize(maximum_high_digit + 1);
        inverse_factorial_.resize(maximum_high_digit + 1);
        factorial_[0] = 1 % modulus_;
        for(std::size_t index = 1; index <= maximum_high_digit; ++index){
            factorial_[index] = multiply(
                factorial_[index - 1], static_cast<u32>(index));
        }
        inverse_factorial_.back() =
            power(factorial_.back(), modulus_ - 2);
        for(std::size_t index = maximum_high_digit; index > 0; --index){
            inverse_factorial_[index - 1] = multiply(
                inverse_factorial_[index], static_cast<u32>(index));
        }
    }

public:
    QBinomialPrimeMod(u32 prime_modulus, u32 q, std::size_t maximum_n)
        : modulus_(prime_modulus), base_(q), maximum_n_(maximum_n){
        if(!is_prime(modulus_)){
            throw std::invalid_argument(
                "QBinomialPrimeMod requires a prime modulus");
        }
        if(base_ >= modulus_){
            throw std::invalid_argument(
                "QBinomialPrimeMod requires q < modulus");
        }
        if(maximum_n_ >= modulus_){
            throw std::invalid_argument(
                "QBinomialPrimeMod requires maximum_n < modulus");
        }
        build_q_factorials();
        build_ordinary_factorials();
    }

    u32 modulus() const noexcept{
        return modulus_;
    }

    u32 base() const noexcept{
        return base_;
    }

    std::size_t maximum_n() const noexcept{
        return maximum_n_;
    }

    std::size_t period() const noexcept{
        return period_;
    }

    u32 binomial(std::size_t n, std::size_t k) const{
        if(n > maximum_n_){
            throw std::out_of_range(
                "QBinomialPrimeMod query exceeds maximum_n");
        }
        if(k > n) return 0;
        if(period_ == 0) return q_binomial_small(n, k);

        const std::size_t n_high = n / period_;
        const std::size_t k_high = k / period_;
        const std::size_t n_low = n % period_;
        const std::size_t k_low = k % period_;
        return multiply(ordinary_binomial(n_high, k_high),
                        q_binomial_small(n_low, k_low));
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_Q_BINOMIAL_PRIME_MOD_HPP_INCLUDED
