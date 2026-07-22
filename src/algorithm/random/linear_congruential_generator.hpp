#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_LINEAR_CONGRUENTIAL_GENERATOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_LINEAR_CONGRUENTIAL_GENERATOR_HPP_INCLUDED

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

template<
    class UInt,
    UInt Multiplier,
    UInt Increment,
    UInt Modulus
>
class LinearCongruentialGenerator{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);
    static_assert(Modulus == 0 || 1 < Modulus);
    static_assert(Modulus == 0 || Multiplier < Modulus);
    static_assert(Modulus == 0 || Increment < Modulus);

public:
    using result_type = UInt;

    static constexpr result_type multiplier = Multiplier;
    static constexpr result_type increment = Increment;
    static constexpr result_type modulus = Modulus;
    static constexpr result_type default_seed = 1;

private:
    using wide_type = std::conditional_t<
        (std::numeric_limits<result_type>::digits <= 32),
        std::uint64_t,
        unsigned __int128
    >;
    result_type state_ = default_seed;

    static constexpr result_type gcd(
        result_type left,
        result_type right
    ) noexcept{
        while(right != 0){
            const result_type remainder = left % right;
            left = right;
            right = remainder;
        }
        return left;
    }

    static constexpr result_type normalize(result_type value) noexcept{
        if constexpr(Modulus == 0) return value;
        else return value % Modulus;
    }

    static constexpr result_type multiply(
        result_type left,
        result_type right
    ) noexcept{
        if constexpr(Modulus == 0){
            return static_cast<result_type>(
                static_cast<std::uint64_t>(left) * right
            );
        }else{
            return static_cast<result_type>(
                static_cast<wide_type>(left) * right % Modulus
            );
        }
    }

    static constexpr result_type add(
        result_type left,
        result_type right
    ) noexcept{
        if constexpr(Modulus == 0){
            return static_cast<result_type>(
                static_cast<std::uint64_t>(left) + right
            );
        }else{
            return static_cast<result_type>(
                (static_cast<wide_type>(left) + right) % Modulus
            );
        }
    }

public:
    constexpr LinearCongruentialGenerator() = default;

    constexpr explicit LinearCongruentialGenerator(result_type seed_value){
        seed(seed_value);
    }

    constexpr void seed(result_type value = default_seed) noexcept{
        state_ = normalize(value);
        if constexpr(Increment == 0){
            if(state_ == 0) state_ = 1;
        }
    }

    static constexpr result_type transition(result_type state) noexcept{
        return add(multiply(Multiplier, normalize(state)), Increment);
    }

    constexpr result_type operator()() noexcept{
        state_ = transition(state_);
        return state_;
    }

    constexpr void discard(unsigned long long count) noexcept{
        result_type accumulated_multiplier = 1;
        result_type accumulated_increment = 0;
        result_type current_multiplier = Multiplier;
        result_type current_increment = Increment;

        while(count != 0){
            if((count & 1ULL) != 0){
                accumulated_increment = add(
                    multiply(current_multiplier, accumulated_increment),
                    current_increment
                );
                accumulated_multiplier = multiply(
                    current_multiplier,
                    accumulated_multiplier
                );
            }
            current_increment = add(
                multiply(current_multiplier, current_increment),
                current_increment
            );
            current_multiplier = multiply(
                current_multiplier,
                current_multiplier
            );
            count >>= 1;
        }
        state_ = add(
            multiply(accumulated_multiplier, state_),
            accumulated_increment
        );
    }

    constexpr result_type state() const noexcept{
        return state_;
    }

    static constexpr result_type min() noexcept{
        if constexpr(Increment != 0){
            return 0;
        }else if constexpr(Modulus == 0){
            return (Multiplier & result_type(1)) != 0 ? 1 : 0;
        }else{
            return gcd(Multiplier, Modulus) == 1 ? 1 : 0;
        }
    }

    static constexpr result_type max() noexcept{
        if constexpr(Modulus == 0){
            return std::numeric_limits<result_type>::max();
        }else{
            return Modulus - 1;
        }
    }
};

using MinStdRand0 = LinearCongruentialGenerator<
    std::uint32_t,
    16807U,
    0U,
    2147483647U
>;

using MinStdRand = LinearCongruentialGenerator<
    std::uint32_t,
    48271U,
    0U,
    2147483647U
>;

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_LINEAR_CONGRUENTIAL_GENERATOR_HPP_INCLUDED
