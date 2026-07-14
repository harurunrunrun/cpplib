#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
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

struct unsigned_129{
    unsigned __int128 low = 0;
    bool high = false;
};

constexpr bool is_zero(unsigned_129 value) noexcept{
    return !value.high && value.low == 0;
}

constexpr bool is_even(unsigned_129 value) noexcept{
    return (value.low & 1) == 0;
}

constexpr int compare(unsigned_129 left, unsigned_129 right) noexcept{
    if(left.high != right.high) return left.high ? 1 : -1;
    if(left.low != right.low) return left.low > right.low ? 1 : -1;
    return 0;
}

constexpr unsigned_129 subtract(
    unsigned_129 left,
    unsigned_129 right
) noexcept{
    const bool borrow = left.low < right.low;
    const unsigned __int128 low = left.low - right.low;
    const unsigned int high = static_cast<unsigned int>(left.high)
        - static_cast<unsigned int>(right.high)
        - static_cast<unsigned int>(borrow);
    return {low, high != 0};
}

constexpr unsigned_129 shift_right(unsigned_129 value) noexcept{
    value.low = (value.low >> 1)
        | (static_cast<unsigned __int128>(value.high) << 127);
    value.high = false;
    return value;
}

constexpr unsigned_129 shift_left(unsigned_129 value) noexcept{
    value.high = value.high || ((value.low >> 127) != 0);
    value.low <<= 1;
    return value;
}

constexpr unsigned_129 binary_gcd(
    unsigned_129 left,
    unsigned_129 right
) noexcept{
    if(is_zero(left)) return right;
    if(is_zero(right)) return left;

    int common_power_of_two = 0;
    while(is_even(left) && is_even(right)){
        left = shift_right(left);
        right = shift_right(right);
        ++common_power_of_two;
    }
    while(is_even(left)) left = shift_right(left);

    do{
        while(is_even(right)) right = shift_right(right);
        if(compare(left, right) > 0){
            const unsigned_129 temporary = left;
            left = right;
            right = temporary;
        }
        right = subtract(right, left);
    }while(!is_zero(right));

    while(common_power_of_two-- > 0) left = shift_left(left);
    return left;
}

template<class UInt>
struct signed_magnitude{
    bool negative;
    UInt magnitude;
};

template<class UInt>
constexpr signed_magnitude<UInt> signed_difference(
    UInt left,
    UInt right
) noexcept{
    return left >= right
        ? signed_magnitude<UInt>{false, static_cast<UInt>(left - right)}
        : signed_magnitude<UInt>{true, static_cast<UInt>(right - left)};
}

constexpr unsigned_129 add_products(
    unsigned __int128 left,
    unsigned __int128 right
) noexcept{
    const unsigned __int128 low = left + right;
    return {low, low < left};
}

template<class UInt>
constexpr unsigned_129 lcg_modulus_invariant(
    signed_magnitude<UInt> first,
    signed_magnitude<UInt> second,
    signed_magnitude<UInt> third
) noexcept{
    const unsigned __int128 outside =
        static_cast<unsigned __int128>(first.magnitude) * third.magnitude;
    const unsigned __int128 middle =
        static_cast<unsigned __int128>(second.magnitude) * second.magnitude;
    if(first.negative != third.negative){
        return add_products(outside, middle);
    }
    return outside >= middle
        ? unsigned_129{outside - middle, false}
        : unsigned_129{middle - outside, false};
}

template<class UInt>
constexpr UInt add(UInt left, UInt right, UInt modulus) noexcept{
    return static_cast<UInt>(
        (static_cast<unsigned __int128>(left) + right) % modulus
    );
}

template<class UInt>
constexpr UInt subtract(UInt left, UInt right, UInt modulus) noexcept{
    return left >= right
        ? static_cast<UInt>(left - right)
        : static_cast<UInt>(modulus - (right - left));
}

template<class UInt>
constexpr UInt multiply(UInt left, UInt right, UInt modulus) noexcept{
    return static_cast<UInt>(
        static_cast<unsigned __int128>(left) * right % modulus
    );
}

template<class UInt>
constexpr std::optional<UInt> inverse(UInt value, UInt modulus) noexcept{
    using signed_wide = __int128_t;
    signed_wide old_remainder = static_cast<signed_wide>(modulus);
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
    old_coefficient %= static_cast<signed_wide>(modulus);
    if(old_coefficient < 0) old_coefficient += modulus;
    return static_cast<UInt>(old_coefficient);
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
    )
        : first_output_(latest_output),
          current_output_(latest_output){
        if constexpr(generator_type::modulus != 0){
            if(latest_output >= generator_type::modulus){
                throw std::invalid_argument(
                    "LCG output must be smaller than its modulus"
                );
            }
        }
    }

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
    if constexpr(Modulus != 0){
        for(const UInt output: consecutive_outputs){
            if(output >= Modulus) return std::nullopt;
        }
    }

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

template<class UInt>
struct LinearCongruentialUnknownModulusCandidate{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);

    UInt modulus;
    UInt multiplier;
    UInt increment;

    constexpr UInt next(UInt state) const noexcept{
        using namespace linear_congruential_cracker_internal;
        return add(
            multiply(multiplier, static_cast<UInt>(state % modulus), modulus),
            increment,
            modulus
        );
    }
};

template<class UInt>
constexpr std::optional<LinearCongruentialUnknownModulusCandidate<UInt>>
recover_linear_congruential_unknown_modulus_candidate(
    std::span<const UInt> consecutive_outputs
) noexcept{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);
    using namespace linear_congruential_cracker_internal;

    if(consecutive_outputs.size() < 4) return std::nullopt;

    unsigned_129 invariant_gcd{};
    auto first = signed_difference(
        consecutive_outputs[1],
        consecutive_outputs[0]
    );
    auto second = signed_difference(
        consecutive_outputs[2],
        consecutive_outputs[1]
    );
    for(std::size_t index = 0; index + 3 < consecutive_outputs.size();
        ++index){
        const auto third = signed_difference(
            consecutive_outputs[index + 3],
            consecutive_outputs[index + 2]
        );
        invariant_gcd = binary_gcd(
            invariant_gcd,
            lcg_modulus_invariant(first, second, third)
        );
        first = second;
        second = third;
    }

    if(is_zero(invariant_gcd) || invariant_gcd.high
       || invariant_gcd.low > std::numeric_limits<UInt>::max()){
        return std::nullopt;
    }
    const UInt modulus = static_cast<UInt>(invariant_gcd.low);
    if(modulus <= 1) return std::nullopt;
    for(const UInt output: consecutive_outputs){
        if(output >= modulus) return std::nullopt;
    }

    std::optional<UInt> multiplier;
    for(std::size_t index = 0; index + 2 < consecutive_outputs.size();
        ++index){
        const UInt difference = subtract(
            consecutive_outputs[index + 1],
            consecutive_outputs[index],
            modulus
        );
        const auto inverse_difference = inverse(difference, modulus);
        if(!inverse_difference) continue;
        const UInt next_difference = subtract(
            consecutive_outputs[index + 2],
            consecutive_outputs[index + 1],
            modulus
        );
        multiplier = multiply(next_difference, *inverse_difference, modulus);
        break;
    }
    if(!multiplier) return std::nullopt;

    LinearCongruentialUnknownModulusCandidate<UInt> result{
        modulus,
        *multiplier,
        subtract(
            consecutive_outputs[1],
            multiply(*multiplier, consecutive_outputs[0], modulus),
            modulus
        ),
    };
    for(std::size_t index = 1; index < consecutive_outputs.size(); ++index){
        if(result.next(consecutive_outputs[index - 1])
            != consecutive_outputs[index]){
            return std::nullopt;
        }
    }
    return result;
}

template<class UInt, std::size_t N>
constexpr std::optional<LinearCongruentialUnknownModulusCandidate<UInt>>
recover_linear_congruential_unknown_modulus_candidate(
    const std::array<UInt, N>& consecutive_outputs
) noexcept{
    return recover_linear_congruential_unknown_modulus_candidate<UInt>(
        std::span<const UInt>(consecutive_outputs)
    );
}
