#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "big_integer.hpp"

struct LenstraEcmParameters{
    std::uint32_t stage1_bound = 20'000;
    std::uint32_t stage2_bound = 400'000;
    std::size_t curves = 48;
    std::uint32_t stage2_wheel = 210;
    std::size_t gcd_batch_size = 128;
    int primality_rounds = 24;
    std::size_t factorization_retries = 8;
    std::uint64_t seed = 0x6a09e667f3bcc909ULL;
};

namespace lenstra_ecm_detail{

using u128 = __uint128_t;
using i128 = __int128_t;

inline u128 gcd_u128(u128 first, u128 second){
    while(second != 0){
        const u128 remainder = first % second;
        first = second;
        second = remainder;
    }
    return first;
}

inline unsigned bit_length_u128(u128 value){
    if(value == 0) return 0;
    const std::uint64_t high = static_cast<std::uint64_t>(value >> 64);
    if(high != 0) return 128U - std::countl_zero(high);
    return 64U - std::countl_zero(static_cast<std::uint64_t>(value));
}

inline u128 random_below(u128 bound, std::mt19937_64& random){
    if(bound == 0){
        throw std::invalid_argument("random_below requires a positive bound");
    }
    const unsigned bits = bit_length_u128(bound - 1);
    while(true){
        u128 value = (static_cast<u128>(random()) << 64) | random();
        if(bits < 128){
            if(bits == 0) value = 0;
            else value &= (u128{1} << bits) - 1;
        }
        if(value < bound) return value;
    }
}

inline std::uint64_t splitmix64(std::uint64_t value){
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

class Montgomery128{
private:
    u128 modulus_;
    std::array<std::uint64_t, 2> modulus_limbs_{};
    std::uint64_t negative_inverse_;
    u128 r_squared_;
    u128 one_;

    static std::uint64_t inverse_odd_word(std::uint64_t value){
        std::uint64_t inverse = 1;
        for(int iteration = 0; iteration < 6; ++iteration){
            inverse *= 2 - value * inverse;
        }
        return inverse;
    }

    u128 double_mod(u128 value) const{
        const u128 doubled = value + value;
        return doubled >= modulus_ ? doubled - modulus_ : doubled;
    }

    u128 multiply_raw(u128 left, u128 right) const{
        const std::array<std::uint64_t, 2> a{
            static_cast<std::uint64_t>(left),
            static_cast<std::uint64_t>(left >> 64)
        };
        const std::array<std::uint64_t, 2> b{
            static_cast<std::uint64_t>(right),
            static_cast<std::uint64_t>(right >> 64)
        };
        std::array<std::uint64_t, 5> temporary{};

        for(std::size_t outer = 0; outer < 2; ++outer){
            std::uint64_t carry = 0;
            for(std::size_t inner = 0; inner < 2; ++inner){
                const u128 current = static_cast<u128>(a[inner]) * b[outer]
                    + temporary[outer + inner] + carry;
                temporary[outer + inner] = static_cast<std::uint64_t>(current);
                carry = static_cast<std::uint64_t>(current >> 64);
            }
            std::size_t position = outer + 2;
            while(carry != 0){
                const u128 current = static_cast<u128>(temporary[position]) + carry;
                temporary[position] = static_cast<std::uint64_t>(current);
                carry = static_cast<std::uint64_t>(current >> 64);
                ++position;
            }
        }

        for(std::size_t outer = 0; outer < 2; ++outer){
            const std::uint64_t multiplier =
                temporary[outer] * negative_inverse_;
            std::uint64_t carry = 0;
            for(std::size_t inner = 0; inner < 2; ++inner){
                const u128 current = static_cast<u128>(multiplier)
                        * modulus_limbs_[inner]
                    + temporary[outer + inner] + carry;
                temporary[outer + inner] = static_cast<std::uint64_t>(current);
                carry = static_cast<std::uint64_t>(current >> 64);
            }
            std::size_t position = outer + 2;
            while(carry != 0){
                const u128 current = static_cast<u128>(temporary[position]) + carry;
                temporary[position] = static_cast<std::uint64_t>(current);
                carry = static_cast<std::uint64_t>(current >> 64);
                ++position;
            }
        }

        u128 result = static_cast<u128>(temporary[2])
            | (static_cast<u128>(temporary[3]) << 64);
        if(temporary[4] != 0 || result >= modulus_){
            result -= modulus_;
        }
        return result;
    }

public:
    explicit Montgomery128(u128 modulus):
        modulus_(modulus),
        modulus_limbs_{
            static_cast<std::uint64_t>(modulus),
            static_cast<std::uint64_t>(modulus >> 64)
        },
        negative_inverse_(0),
        r_squared_(0),
        one_(0){
        if(modulus < 3 || (modulus & 1) == 0
            || bit_length_u128(modulus) > 127){
            throw std::invalid_argument(
                "Montgomery128 requires an odd modulus below 2^127"
            );
        }
        negative_inverse_ = 0 - inverse_odd_word(modulus_limbs_[0]);
        u128 r_modulus = 1;
        for(int bit = 0; bit < 128; ++bit) r_modulus = double_mod(r_modulus);
        one_ = r_modulus;
        r_squared_ = r_modulus;
        for(int bit = 0; bit < 128; ++bit) r_squared_ = double_mod(r_squared_);
    }

    u128 modulus() const noexcept{ return modulus_; }
    u128 zero() const noexcept{ return 0; }
    u128 one() const noexcept{ return one_; }

    u128 encode(u128 value) const{
        return multiply_raw(value % modulus_, r_squared_);
    }

    u128 decode(u128 value) const{
        return multiply_raw(value, 1);
    }

    u128 add(u128 left, u128 right) const{
        const u128 result = left + right;
        return result >= modulus_ ? result - modulus_ : result;
    }

    u128 subtract(u128 left, u128 right) const{
        return left >= right ? left - right : modulus_ - (right - left);
    }

    u128 multiply(u128 left, u128 right) const{
        return multiply_raw(left, right);
    }

    u128 square(u128 value) const{
        return multiply_raw(value, value);
    }

    u128 power(u128 base, u128 exponent) const{
        u128 result = one_;
        while(exponent != 0){
            if((exponent & 1) != 0) result = multiply(result, base);
            base = square(base);
            exponent >>= 1;
        }
        return result;
    }
};

inline std::optional<u128> inverse_mod_odd(u128 value, u128 modulus){
    value %= modulus;
    if(value == 0 || gcd_u128(value, modulus) != 1) return std::nullopt;
    u128 first = value;
    u128 second = modulus;
    u128 first_coefficient = 1;
    u128 second_coefficient = 0;
    while(first != 1 && second != 1){
        while((first & 1) == 0){
            first >>= 1;
            first_coefficient = (first_coefficient & 1) == 0
                ? first_coefficient >> 1
                : (first_coefficient + modulus) >> 1;
        }
        while((second & 1) == 0){
            second >>= 1;
            second_coefficient = (second_coefficient & 1) == 0
                ? second_coefficient >> 1
                : (second_coefficient + modulus) >> 1;
        }
        if(first >= second){
            first -= second;
            first_coefficient = first_coefficient >= second_coefficient
                ? first_coefficient - second_coefficient
                : modulus - (second_coefficient - first_coefficient);
        }else{
            second -= first;
            second_coefficient = second_coefficient >= first_coefficient
                ? second_coefficient - first_coefficient
                : modulus - (first_coefficient - second_coefficient);
        }
    }
    return first == 1 ? first_coefficient : second_coefficient;
}

inline std::vector<std::uint32_t> prime_sieve(std::uint32_t limit){
    std::vector<bool> prime(static_cast<std::size_t>(limit) + 1, true);
    prime[0] = false;
    if(limit >= 1) prime[1] = false;
    for(std::uint32_t value = 2;
        static_cast<std::uint64_t>(value) * value <= limit;
        ++value){
        if(!prime[value]) continue;
        for(std::uint64_t multiple = static_cast<std::uint64_t>(value) * value;
            multiple <= limit;
            multiple += value){
            prime[static_cast<std::size_t>(multiple)] = false;
        }
    }
    std::vector<std::uint32_t> result;
    for(std::uint32_t value = 2; value <= limit; ++value){
        if(prime[value]) result.push_back(value);
    }
    return result;
}

inline bool is_probable_prime(
    u128 value,
    int rounds,
    std::mt19937_64& random
){
    if(value < 2) return false;
    constexpr std::array<std::uint32_t, 16> small_primes{
        2, 3, 5, 7, 11, 13, 17, 19,
        23, 29, 31, 37, 41, 43, 47, 53
    };
    for(const std::uint32_t prime: small_primes){
        if(value % prime == 0) return value == prime;
    }
    Montgomery128 arithmetic(value);
    u128 odd_part = value - 1;
    unsigned powers_of_two = 0;
    while((odd_part & 1) == 0){
        odd_part >>= 1;
        ++powers_of_two;
    }
    for(int round = 0; round < rounds; ++round){
        const u128 base = random_below(value - 3, random) + 2;
        u128 current = arithmetic.power(arithmetic.encode(base), odd_part);
        if(current == arithmetic.one()
            || current == arithmetic.encode(value - 1)) continue;
        bool witness = true;
        for(unsigned exponent = 1; exponent < powers_of_two; ++exponent){
            current = arithmetic.square(current);
            if(current == arithmetic.encode(value - 1)){
                witness = false;
                break;
            }
        }
        if(witness) return false;
    }
    return true;
}

struct MontgomeryPoint{
    u128 x;
    u128 z;
};

inline MontgomeryPoint point_double(
    const MontgomeryPoint& point,
    u128 a24,
    const Montgomery128& arithmetic
){
    const u128 plus = arithmetic.add(point.x, point.z);
    const u128 minus = arithmetic.subtract(point.x, point.z);
    const u128 plus_squared = arithmetic.square(plus);
    const u128 minus_squared = arithmetic.square(minus);
    const u128 difference = arithmetic.subtract(plus_squared, minus_squared);
    return {
        arithmetic.multiply(plus_squared, minus_squared),
        arithmetic.multiply(
            difference,
            arithmetic.add(
                minus_squared,
                arithmetic.multiply(a24, difference)
            )
        )
    };
}

inline MontgomeryPoint point_add(
    const MontgomeryPoint& first,
    const MontgomeryPoint& second,
    const MontgomeryPoint& difference,
    const Montgomery128& arithmetic
){
    const u128 first_plus = arithmetic.add(first.x, first.z);
    const u128 first_minus = arithmetic.subtract(first.x, first.z);
    const u128 second_plus = arithmetic.add(second.x, second.z);
    const u128 second_minus = arithmetic.subtract(second.x, second.z);
    const u128 diagonal = arithmetic.multiply(first_plus, second_minus);
    const u128 cross = arithmetic.multiply(first_minus, second_plus);
    return {
        arithmetic.multiply(
            arithmetic.square(arithmetic.add(diagonal, cross)),
            difference.z
        ),
        arithmetic.multiply(
            arithmetic.square(arithmetic.subtract(diagonal, cross)),
            difference.x
        )
    };
}

inline MontgomeryPoint scalar_multiply(
    MontgomeryPoint point,
    std::uint64_t scalar,
    u128 a24,
    const Montgomery128& arithmetic
){
    if(scalar == 0) return {arithmetic.one(), arithmetic.zero()};
    if(scalar == 1) return point;
    MontgomeryPoint lower = point;
    MontgomeryPoint upper = point_double(point, a24, arithmetic);
    const unsigned highest_bit = 63U - std::countl_zero(scalar);
    for(unsigned bit = highest_bit; bit-- > 0;){
        if(((scalar >> bit) & 1U) == 0){
            upper = point_add(lower, upper, point, arithmetic);
            lower = point_double(lower, a24, arithmetic);
        }else{
            lower = point_add(lower, upper, point, arithmetic);
            upper = point_double(upper, a24, arithmetic);
        }
    }
    return lower;
}

struct CurveSetup{
    MontgomeryPoint point;
    u128 a24;
};

struct CurveSetupResult{
    std::optional<CurveSetup> curve;
    u128 factor = 1;
};

inline CurveSetupResult make_suyama_curve(
    u128 modulus,
    Montgomery128& arithmetic,
    std::mt19937_64& random
){
    const u128 sigma_value = random_below(modulus - 6, random) + 6;
    const u128 sigma = arithmetic.encode(sigma_value);
    const u128 five = arithmetic.encode(5);
    const u128 four = arithmetic.encode(4);
    const u128 three = arithmetic.encode(3);
    const u128 sixteen = arithmetic.encode(16);
    const u128 u = arithmetic.subtract(arithmetic.square(sigma), five);
    const u128 v = arithmetic.multiply(four, sigma);
    const u128 u_cubed = arithmetic.multiply(arithmetic.square(u), u);
    const u128 v_cubed = arithmetic.multiply(arithmetic.square(v), v);
    const u128 v_minus_u = arithmetic.subtract(v, u);
    const u128 numerator = arithmetic.multiply(
        arithmetic.multiply(arithmetic.square(v_minus_u), v_minus_u),
        arithmetic.add(arithmetic.multiply(three, u), v)
    );
    const u128 denominator = arithmetic.multiply(
        arithmetic.multiply(sixteen, u_cubed),
        v
    );
    const u128 decoded_denominator = arithmetic.decode(denominator);
    const u128 denominator_gcd = gcd_u128(decoded_denominator, modulus);
    if(denominator_gcd != 1){
        return {std::nullopt, denominator_gcd};
    }
    const std::optional<u128> inverse =
        inverse_mod_odd(decoded_denominator, modulus);
    if(!inverse.has_value()) return {std::nullopt, modulus};
    const u128 a24 = arithmetic.multiply(
        numerator,
        arithmetic.encode(*inverse)
    );
    const u128 curve_a = arithmetic.subtract(
        arithmetic.multiply(arithmetic.encode(4), a24),
        arithmetic.encode(2)
    );
    const u128 discriminant = arithmetic.subtract(
        arithmetic.square(curve_a),
        arithmetic.encode(4)
    );
    const u128 discriminant_gcd =
        gcd_u128(arithmetic.decode(discriminant), modulus);
    if(discriminant_gcd != 1){
        return {std::nullopt, discriminant_gcd};
    }
    return {CurveSetup{{u_cubed, v_cubed}, a24}, 1};
}

inline u128 stage1(
    const CurveSetup& curve,
    u128 modulus,
    const std::vector<std::uint32_t>& primes,
    std::uint32_t bound,
    const Montgomery128& arithmetic,
    MontgomeryPoint& result
){
    result = curve.point;
    std::vector<u128> checkpoints;
    for(const std::uint32_t prime: primes){
        if(prime > bound) break;
        std::uint64_t prime_power = prime;
        while(prime_power <= bound / prime) prime_power *= prime;
        result = scalar_multiply(result, prime_power, curve.a24, arithmetic);
        checkpoints.push_back(arithmetic.decode(result.z));
    }
    const u128 divisor = gcd_u128(arithmetic.decode(result.z), modulus);
    if(divisor != modulus) return divisor;
    for(const u128 checkpoint: checkpoints){
        const u128 checkpoint_gcd = gcd_u128(checkpoint, modulus);
        if(checkpoint_gcd > 1 && checkpoint_gcd < modulus){
            return checkpoint_gcd;
        }
    }
    return modulus;
}

class BatchedGcd{
private:
    const Montgomery128& arithmetic_;
    u128 modulus_;
    std::size_t batch_size_;
    u128 product_;
    std::vector<u128> values_;

public:
    BatchedGcd(
        const Montgomery128& arithmetic,
        u128 modulus,
        std::size_t batch_size
    ):
        arithmetic_(arithmetic),
        modulus_(modulus),
        batch_size_(batch_size),
        product_(arithmetic.one()){
        values_.reserve(batch_size);
    }

    u128 flush(){
        if(values_.empty()) return 1;
        const u128 divisor = gcd_u128(arithmetic_.decode(product_), modulus_);
        if(divisor > 1 && divisor < modulus_) return divisor;
        if(divisor == modulus_){
            for(const u128 value: values_){
                const u128 individual =
                    gcd_u128(arithmetic_.decode(value), modulus_);
                if(individual > 1 && individual < modulus_) return individual;
            }
        }
        product_ = arithmetic_.one();
        values_.clear();
        return 1;
    }

    u128 add(u128 value){
        product_ = arithmetic_.multiply(product_, value);
        values_.push_back(value);
        return values_.size() >= batch_size_ ? flush() : 1;
    }
};

inline u128 stage2(
    const MontgomeryPoint& stage1_point,
    u128 a24,
    u128 modulus,
    const std::vector<std::uint32_t>& primes,
    const LenstraEcmParameters& parameters,
    const Montgomery128& arithmetic
){
    if(parameters.stage2_bound <= parameters.stage1_bound) return 1;
    const std::uint32_t wheel = parameters.stage2_wheel;
    const std::size_t baby_count = static_cast<std::size_t>(wheel / 2) + 1;
    std::vector<std::optional<MontgomeryPoint>> babies(baby_count);
    struct Stage2Entry{
        std::uint32_t giant;
        std::uint32_t baby;
        std::uint32_t prime;
    };
    std::vector<Stage2Entry> entries;
    for(const std::uint32_t prime: primes){
        if(prime <= parameters.stage1_bound) continue;
        if(prime > parameters.stage2_bound) break;
        const std::uint32_t remainder = prime % wheel;
        if(remainder == 0){
            entries.push_back({0, 0, prime});
            continue;
        }
        if(remainder <= wheel / 2){
            entries.push_back({prime / wheel, remainder, 0});
        }else{
            entries.push_back({prime / wheel + 1, wheel - remainder, 0});
        }
    }
    for(const Stage2Entry& entry: entries){
        if(entry.baby != 0 && !babies[entry.baby].has_value()){
            babies[entry.baby] = scalar_multiply(
                stage1_point,
                entry.baby,
                a24,
                arithmetic
            );
        }
    }

    BatchedGcd batched(arithmetic, modulus, parameters.gcd_batch_size);
    MontgomeryPoint wheel_point = scalar_multiply(
        stage1_point,
        wheel,
        a24,
        arithmetic
    );
    MontgomeryPoint previous{arithmetic.one(), arithmetic.zero()};
    MontgomeryPoint current = wheel_point;
    std::uint32_t current_giant = 0;
    for(const Stage2Entry& entry: entries){
        if(entry.prime != 0){
            const MontgomeryPoint direct = scalar_multiply(
                stage1_point,
                entry.prime,
                a24,
                arithmetic
            );
            const u128 divisor = batched.add(direct.z);
            if(divisor != 1) return divisor;
            continue;
        }
        while(current_giant < entry.giant){
            ++current_giant;
            if(current_giant == 1) continue;
            const MontgomeryPoint next = current_giant == 2
                ? point_double(wheel_point, a24, arithmetic)
                : point_add(current, wheel_point, previous, arithmetic);
            previous = current;
            current = next;
        }
        const MontgomeryPoint giant = entry.giant == 0
            ? MontgomeryPoint{arithmetic.one(), arithmetic.zero()}
            : current;
        const MontgomeryPoint& baby = *babies[entry.baby];
        const u128 difference = arithmetic.subtract(
            arithmetic.multiply(giant.x, baby.z),
            arithmetic.multiply(baby.x, giant.z)
        );
        const u128 divisor = batched.add(difference);
        if(divisor != 1) return divisor;
    }
    return batched.flush();
}

inline void validate_parameters(const LenstraEcmParameters& parameters){
    if(parameters.stage1_bound < 2
        || parameters.stage2_bound < parameters.stage1_bound
        || parameters.stage2_wheel < 2
        || parameters.gcd_batch_size == 0
        || parameters.primality_rounds <= 0
        || parameters.factorization_retries == 0){
        throw std::invalid_argument("invalid Lenstra ECM parameters");
    }
}

inline std::optional<u128> find_factor_u128(
    u128 value,
    const LenstraEcmParameters& parameters
){
    validate_parameters(parameters);
    if(value < 4) return std::nullopt;
    constexpr std::array<std::uint32_t, 25> trial_primes{
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
        43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
    };
    for(const std::uint32_t prime: trial_primes){
        if(value % prime == 0){
            return value == prime ? std::nullopt
                                  : std::optional<u128>(prime);
        }
    }
    std::mt19937_64 random(parameters.seed);
    if(is_probable_prime(value, parameters.primality_rounds, random)){
        return std::nullopt;
    }
    Montgomery128 arithmetic(value);
    const std::vector<std::uint32_t> primes =
        prime_sieve(parameters.stage2_bound);
    for(std::size_t curve_index = 0;
        curve_index < parameters.curves;
        ++curve_index){
        const CurveSetupResult setup =
            make_suyama_curve(value, arithmetic, random);
        if(setup.factor > 1 && setup.factor < value) return setup.factor;
        if(!setup.curve.has_value()) continue;
        MontgomeryPoint stage1_point{};
        const u128 first_divisor = stage1(
            *setup.curve,
            value,
            primes,
            parameters.stage1_bound,
            arithmetic,
            stage1_point
        );
        if(first_divisor > 1 && first_divisor < value) return first_divisor;
        if(first_divisor == value) continue;
        const u128 second_divisor = stage2(
            stage1_point,
            setup.curve->a24,
            value,
            primes,
            parameters,
            arithmetic
        );
        if(second_divisor > 1 && second_divisor < value) return second_divisor;
    }
    return std::nullopt;
}

inline void factorize_recursive(
    u128 value,
    LenstraEcmParameters parameters,
    std::vector<u128>& factors,
    std::uint64_t salt
){
    if(value == 1) return;
    std::mt19937_64 primality_random(splitmix64(parameters.seed ^ salt));
    if(is_probable_prime(value, parameters.primality_rounds, primality_random)){
        factors.push_back(value);
        return;
    }
    std::optional<u128> divisor;
    for(std::size_t retry = 0;
        retry < parameters.factorization_retries && !divisor.has_value();
        ++retry){
        parameters.seed = splitmix64(parameters.seed + retry + salt);
        divisor = find_factor_u128(value, parameters);
    }
    if(!divisor.has_value()){
        throw std::runtime_error(
            "Lenstra ECM exhausted the configured curve budget"
        );
    }
    factorize_recursive(
        *divisor,
        parameters,
        factors,
        splitmix64(salt + 1)
    );
    factorize_recursive(
        value / *divisor,
        parameters,
        factors,
        splitmix64(salt + 2)
    );
}

inline u128 to_supported_positive(const BigInteger& value){
    const BigInteger positive = value.absolute();
    if(positive.bit_length() > 127){
        throw std::overflow_error(
            "Lenstra ECM currently supports magnitudes below 2^127"
        );
    }
    return positive.checked_to<u128>();
}

}  // namespace lenstra_ecm_detail

inline std::optional<BigInteger> lenstra_ecm_find_factor(
    const BigInteger& value,
    const LenstraEcmParameters& parameters = {}
){
    const lenstra_ecm_detail::u128 positive =
        lenstra_ecm_detail::to_supported_positive(value);
    const std::optional<lenstra_ecm_detail::u128> factor =
        lenstra_ecm_detail::find_factor_u128(positive, parameters);
    if(!factor.has_value()) return std::nullopt;
    return BigInteger(*factor);
}

inline std::vector<std::pair<BigInteger, int>> factorize_lenstra_ecm(
    const BigInteger& value,
    const LenstraEcmParameters& parameters = {}
){
    lenstra_ecm_detail::validate_parameters(parameters);
    const lenstra_ecm_detail::u128 positive =
        lenstra_ecm_detail::to_supported_positive(value);
    if(positive == 0){
        throw std::invalid_argument("factorization of zero is undefined");
    }
    std::vector<lenstra_ecm_detail::u128> flat_factors;
    lenstra_ecm_detail::factorize_recursive(
        positive,
        parameters,
        flat_factors,
        0x243f6a8885a308d3ULL
    );
    std::sort(flat_factors.begin(), flat_factors.end());
    std::vector<std::pair<BigInteger, int>> result;
    for(const lenstra_ecm_detail::u128 factor: flat_factors){
        const BigInteger converted(factor);
        if(result.empty() || result.back().first != converted){
            result.emplace_back(converted, 1);
        }else{
            ++result.back().second;
        }
    }
    return result;
}
