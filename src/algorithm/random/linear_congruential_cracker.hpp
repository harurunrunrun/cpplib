#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <type_traits>

#include "linear_congruential_generator.hpp"

namespace linear_congruential_cracker_internal{

template<class UInt>
using wide_type = std::conditional_t<
    (std::numeric_limits<UInt>::digits <= 32),
    std::uint64_t,
    unsigned __int128
>;

template<class UInt, UInt Modulus>
constexpr UInt add(UInt left, UInt right) noexcept{
    if constexpr(Modulus == 0){
        return static_cast<UInt>(static_cast<std::uint64_t>(left) + right);
    }else{
        return static_cast<UInt>(
            (static_cast<wide_type<UInt>>(left) + right) % Modulus
        );
    }
}

template<class UInt, UInt Modulus>
constexpr UInt subtract(UInt left, UInt right) noexcept{
    if constexpr(Modulus == 0){
        return static_cast<UInt>(
            static_cast<std::uint64_t>(left) - right
        );
    }else{
        return left >= right
            ? static_cast<UInt>(left - right)
            : static_cast<UInt>(Modulus - (right - left));
    }
}

template<class UInt, UInt Modulus>
constexpr UInt multiply(UInt left, UInt right) noexcept{
    if constexpr(Modulus == 0){
        return static_cast<UInt>(
            static_cast<std::uint64_t>(left) * right
        );
    }else{
        return static_cast<UInt>(
            static_cast<wide_type<UInt>>(left) * right % Modulus
        );
    }
}

template<class UInt>
constexpr std::optional<UInt> inverse_power_of_two(UInt value) noexcept{
    if((value & UInt(1)) == 0) return std::nullopt;
    UInt inverse = 1;
    for(int bits = 1; bits < std::numeric_limits<UInt>::digits; bits <<= 1){
        const UInt product = multiply<UInt, static_cast<UInt>(0)>(
            value,
            inverse
        );
        const UInt correction = subtract<UInt, static_cast<UInt>(0)>(
            UInt(2),
            product
        );
        inverse = multiply<UInt, static_cast<UInt>(0)>(inverse, correction);
    }
    return inverse;
}

template<class UInt, UInt Modulus>
constexpr std::optional<UInt> inverse(UInt value) noexcept{
    if constexpr(Modulus == 0){
        return inverse_power_of_two(value);
    }else{
        using signed_wide = __int128_t;
        signed_wide old_remainder = static_cast<signed_wide>(Modulus);
        signed_wide remainder = static_cast<signed_wide>(value);
        signed_wide old_coefficient = 0;
        signed_wide coefficient = 1;
        while(remainder != 0){
            const signed_wide quotient = old_remainder / remainder;
            const signed_wide next_remainder =
                old_remainder - quotient * remainder;
            old_remainder = remainder;
            remainder = next_remainder;
            const signed_wide next_coefficient =
                old_coefficient - quotient * coefficient;
            old_coefficient = coefficient;
            coefficient = next_coefficient;
        }
        if(old_remainder != 1) return std::nullopt;
        old_coefficient %= static_cast<signed_wide>(Modulus);
        if(old_coefficient < 0) old_coefficient += Modulus;
        return static_cast<UInt>(old_coefficient);
    }
}

}

template<class Generator>
class LinearCongruentialCracker{
public:
    using generator_type = Generator;
    using result_type = typename generator_type::result_type;

    static_assert(std::is_unsigned_v<result_type>);
    static_assert(std::numeric_limits<result_type>::digits <= 64);

private:
    result_type first_output_;
    result_type current_output_;

public:
    constexpr explicit LinearCongruentialCracker(
        result_type latest_output
    ) noexcept
        : first_output_(latest_output),
          current_output_(latest_output){}

    constexpr result_type operator()() noexcept{
        return next();
    }

    constexpr result_type next() noexcept{
        current_output_ = generator_type::transition(current_output_);
        return current_output_;
    }

    constexpr result_type current_output() const noexcept{
        return current_output_;
    }

    constexpr std::optional<result_type> recovered_seed() const noexcept{
        using namespace linear_congruential_cracker_internal;
        constexpr result_type modulus = generator_type::modulus;
        const auto inverse_multiplier =
            inverse<result_type, modulus>(generator_type::multiplier);
        if(!inverse_multiplier) return std::nullopt;
        const result_type without_increment =
            subtract<result_type, modulus>(
                first_output_,
                generator_type::increment
            );
        return multiply<result_type, modulus>(
            *inverse_multiplier,
            without_increment
        );
    }
};

template<class UInt, UInt Modulus>
struct LinearCongruentialParameters{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);
    static_assert(Modulus == 0 || 1 < Modulus);

    UInt multiplier;
    UInt increment;

    constexpr UInt next(UInt state) const noexcept{
        using namespace linear_congruential_cracker_internal;
        return add<UInt, Modulus>(
            multiply<UInt, Modulus>(multiplier, state),
            increment
        );
    }
};

template<class UInt, UInt Modulus, std::size_t N>
constexpr std::optional<LinearCongruentialParameters<UInt, Modulus>>
recover_linear_congruential_parameters(
    const std::array<UInt, N>& consecutive_outputs
) noexcept{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(N >= 3);
    using namespace linear_congruential_cracker_internal;

    const UInt first_difference = subtract<UInt, Modulus>(
        consecutive_outputs[1],
        consecutive_outputs[0]
    );
    const UInt second_difference = subtract<UInt, Modulus>(
        consecutive_outputs[2],
        consecutive_outputs[1]
    );
    const auto inverse_difference =
        inverse<UInt, Modulus>(first_difference);
    if(!inverse_difference) return std::nullopt;

    LinearCongruentialParameters<UInt, Modulus> result{
        multiply<UInt, Modulus>(
            second_difference,
            *inverse_difference
        ),
        0,
    };
    result.increment = subtract<UInt, Modulus>(
        consecutive_outputs[1],
        multiply<UInt, Modulus>(
            result.multiplier,
            consecutive_outputs[0]
        )
    );

    for(std::size_t index = 1; index < N; ++index){
        if(result.next(consecutive_outputs[index - 1])
            != consecutive_outputs[index]){
            return std::nullopt;
        }
    }
    return result;
}
