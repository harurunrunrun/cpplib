#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTEGER_EXACT_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTEGER_EXACT_INTEGER_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace exact_integer_detail{

template<class Integer>
inline constexpr bool native_integer =
    std::is_integral_v<std::remove_cv_t<Integer>>
#if defined(__SIZEOF_INT128__)
    || std::same_as<std::remove_cv_t<Integer>, __int128_t>
    || std::same_as<std::remove_cv_t<Integer>, __uint128_t>
#endif
    ;

template<class Integer>
concept NativeInteger = native_integer<Integer>;

template<class Integer>
struct MakeUnsigned{
    using type = std::make_unsigned_t<Integer>;
};

#if defined(__SIZEOF_INT128__)
template<>
struct MakeUnsigned<__int128_t>{
    using type = __uint128_t;
};

template<>
struct MakeUnsigned<__uint128_t>{
    using type = __uint128_t;
};
#endif

template<class Integer>
using MakeUnsignedT = typename MakeUnsigned<std::remove_cv_t<Integer>>::type;

}  // namespace exact_integer_detail

class ExactInteger{
    static constexpr std::uint64_t limb_base = std::uint64_t{1} << 32;
    static constexpr std::uint64_t decimal_base = 1'000'000'000;

    using DecimalMagnitude = std::vector<std::uint32_t>;

    std::vector<std::uint32_t> limbs_;
    bool negative_ = false;

    void normalize(){
        while(!limbs_.empty() && limbs_.back() == 0) limbs_.pop_back();
        if(limbs_.empty()) negative_ = false;
    }

    template<exact_integer_detail::NativeInteger Integer>
    void assign_integral(Integer value){
        limbs_.clear();
        negative_ = false;
        using Value = std::remove_cv_t<Integer>;
        if constexpr(std::same_as<Value, bool>){
            if(value) limbs_.push_back(1);
            return;
        }else{
            using Unsigned = exact_integer_detail::MakeUnsignedT<Value>;
            Unsigned magnitude = static_cast<Unsigned>(value);
            if constexpr(std::numeric_limits<Value>::is_signed){
                if(value < 0){
                    negative_ = true;
                    magnitude = Unsigned{0} - magnitude;
                }
            }
            while(magnitude != 0){
                limbs_.push_back(static_cast<std::uint32_t>(magnitude));
                if constexpr(std::numeric_limits<Unsigned>::digits > 32){
                    magnitude >>= 32;
                }else{
                    magnitude = 0;
                }
            }
        }
    }

    static int compare_magnitude(
        const ExactInteger& left,
        const ExactInteger& right
    ){
        if(left.limbs_.size() != right.limbs_.size()){
            return left.limbs_.size() < right.limbs_.size() ? -1 : 1;
        }
        for(std::size_t index = left.limbs_.size(); index-- > 0;){
            if(left.limbs_[index] != right.limbs_[index]){
                return left.limbs_[index] < right.limbs_[index] ? -1 : 1;
            }
        }
        return 0;
    }

    static std::vector<std::uint32_t> add_magnitudes(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        const std::size_t size = std::max(left.size(), right.size());
        std::vector<std::uint32_t> result(size + 1, 0);
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < size; ++index){
            const std::uint64_t sum = carry
                + (index < left.size() ? left[index] : 0)
                + (index < right.size() ? right[index] : 0);
            result[index] = static_cast<std::uint32_t>(sum);
            carry = sum >> 32;
        }
        result[size] = static_cast<std::uint32_t>(carry);
        return result;
    }

    static std::vector<std::uint32_t> subtract_magnitudes(
        const std::vector<std::uint32_t>& larger,
        const std::vector<std::uint32_t>& smaller
    ){
        std::vector<std::uint32_t> result(larger.size(), 0);
        std::uint64_t borrow = 0;
        for(std::size_t index = 0; index < larger.size(); ++index){
            const std::uint64_t subtrahend = borrow
                + (index < smaller.size() ? smaller[index] : 0);
            const std::uint64_t minuend = larger[index];
            if(minuend < subtrahend){
                result[index] = static_cast<std::uint32_t>(
                    minuend + limb_base - subtrahend
                );
                borrow = 1;
            }else{
                result[index] = static_cast<std::uint32_t>(
                    minuend - subtrahend
                );
                borrow = 0;
            }
        }
        return result;
    }

    static void trim_magnitude(std::vector<std::uint32_t>& value){
        while(!value.empty() && value.back() == 0) value.pop_back();
    }

    struct SignedMagnitude{
        std::vector<std::uint32_t> magnitude;
        bool negative = false;
    };

    static int compare_magnitude_vectors(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        if(left.size() != right.size()){
            return left.size() < right.size() ? -1 : 1;
        }
        for(std::size_t index = left.size(); index-- > 0;){
            if(left[index] != right[index]){
                return left[index] < right[index] ? -1 : 1;
            }
        }
        return 0;
    }

    static void normalize_signed_magnitude(SignedMagnitude& value){
        trim_magnitude(value.magnitude);
        if(value.magnitude.empty()) value.negative = false;
    }

    static SignedMagnitude signed_magnitude(
        const std::vector<std::uint32_t>& magnitude
    ){
        SignedMagnitude result{magnitude, false};
        normalize_signed_magnitude(result);
        return result;
    }

    static SignedMagnitude negate_signed_magnitude(SignedMagnitude value){
        if(!value.magnitude.empty()) value.negative = !value.negative;
        return value;
    }

    static SignedMagnitude add_signed_magnitudes(
        const SignedMagnitude& left,
        const SignedMagnitude& right
    ){
        if(left.magnitude.empty()) return right;
        if(right.magnitude.empty()) return left;
        SignedMagnitude result;
        if(left.negative == right.negative){
            result.magnitude = add_magnitudes(
                left.magnitude,
                right.magnitude
            );
            result.negative = left.negative;
        }else{
            const int order = compare_magnitude_vectors(
                left.magnitude,
                right.magnitude
            );
            if(order == 0) return {};
            if(order > 0){
                result.magnitude = subtract_magnitudes(
                    left.magnitude,
                    right.magnitude
                );
                result.negative = left.negative;
            }else{
                result.magnitude = subtract_magnitudes(
                    right.magnitude,
                    left.magnitude
                );
                result.negative = right.negative;
            }
        }
        normalize_signed_magnitude(result);
        return result;
    }

    static SignedMagnitude subtract_signed_magnitudes(
        const SignedMagnitude& left,
        SignedMagnitude right
    ){
        return add_signed_magnitudes(
            left,
            negate_signed_magnitude(std::move(right))
        );
    }

    static SignedMagnitude multiply_signed_magnitude_small(
        const SignedMagnitude& value,
        std::uint32_t factor
    ){
        if(value.magnitude.empty() || factor == 0) return {};
        SignedMagnitude result;
        result.magnitude.resize(value.magnitude.size() + 1, 0);
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < value.magnitude.size(); ++index){
            const std::uint64_t current =
                static_cast<std::uint64_t>(value.magnitude[index]) * factor
                + carry;
            result.magnitude[index] = static_cast<std::uint32_t>(current);
            carry = current >> 32;
        }
        result.magnitude[value.magnitude.size()] =
            static_cast<std::uint32_t>(carry);
        result.negative = value.negative;
        normalize_signed_magnitude(result);
        return result;
    }

    static SignedMagnitude divide_signed_magnitude_exact(
        SignedMagnitude value,
        std::uint32_t divisor
    ){
        std::uint64_t remainder = 0;
        for(std::size_t index = value.magnitude.size(); index-- > 0;){
            const std::uint64_t current =
                (remainder << 32) | value.magnitude[index];
            value.magnitude[index] =
                static_cast<std::uint32_t>(current / divisor);
            remainder = current % divisor;
        }
        if(remainder != 0){
            throw std::logic_error(
                "ExactInteger Toom-Cook interpolation was not exact"
            );
        }
        normalize_signed_magnitude(value);
        return value;
    }

    static SignedMagnitude multiply_signed_magnitudes(
        const SignedMagnitude& left,
        const SignedMagnitude& right
    ){
        SignedMagnitude result;
        result.magnitude = multiply_magnitudes(
            left.magnitude,
            right.magnitude
        );
        result.negative = !result.magnitude.empty()
            && left.negative != right.negative;
        return result;
    }

    static std::vector<std::uint32_t> toom_cook_3_multiply(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        const std::size_t longer = std::max(left.size(), right.size());
        const std::size_t split = longer / 3 + (longer % 3 != 0);

        const SignedMagnitude left_0 = signed_magnitude(
            limb_range(left, 0, split)
        );
        const SignedMagnitude left_1 = signed_magnitude(
            limb_range(left, split, split * 2)
        );
        const SignedMagnitude left_2 = signed_magnitude(
            limb_range(left, split * 2, left.size())
        );
        const SignedMagnitude right_0 = signed_magnitude(
            limb_range(right, 0, split)
        );
        const SignedMagnitude right_1 = signed_magnitude(
            limb_range(right, split, split * 2)
        );
        const SignedMagnitude right_2 = signed_magnitude(
            limb_range(right, split * 2, right.size())
        );

        const SignedMagnitude left_at_1 = add_signed_magnitudes(
            add_signed_magnitudes(left_0, left_1),
            left_2
        );
        const SignedMagnitude right_at_1 = add_signed_magnitudes(
            add_signed_magnitudes(right_0, right_1),
            right_2
        );
        const SignedMagnitude left_at_minus_1 = add_signed_magnitudes(
            subtract_signed_magnitudes(left_0, left_1),
            left_2
        );
        const SignedMagnitude right_at_minus_1 = add_signed_magnitudes(
            subtract_signed_magnitudes(right_0, right_1),
            right_2
        );
        const SignedMagnitude left_at_2 = add_signed_magnitudes(
            add_signed_magnitudes(
                left_0,
                multiply_signed_magnitude_small(left_1, 2)
            ),
            multiply_signed_magnitude_small(left_2, 4)
        );
        const SignedMagnitude right_at_2 = add_signed_magnitudes(
            add_signed_magnitudes(
                right_0,
                multiply_signed_magnitude_small(right_1, 2)
            ),
            multiply_signed_magnitude_small(right_2, 4)
        );

        const SignedMagnitude value_0 = multiply_signed_magnitudes(
            left_0,
            right_0
        );
        const SignedMagnitude value_1 = multiply_signed_magnitudes(
            left_at_1,
            right_at_1
        );
        const SignedMagnitude value_minus_1 = multiply_signed_magnitudes(
            left_at_minus_1,
            right_at_minus_1
        );
        const SignedMagnitude value_2 = multiply_signed_magnitudes(
            left_at_2,
            right_at_2
        );
        const SignedMagnitude value_infinity = multiply_signed_magnitudes(
            left_2,
            right_2
        );

        // For c0+c1*x+...+c4*x^4, (v1-v(-1))/2 is c1+c3.
        // The even coefficients follow from (v1+v(-1))/2, and v2 then
        // separates c1 from c3.  Every division below is exact over the
        // integers, including when an evaluation value is negative.

        const SignedMagnitude odd_sum = divide_signed_magnitude_exact(
            subtract_signed_magnitudes(value_1, value_minus_1),
            2
        );
        SignedMagnitude coefficient_2 = subtract_signed_magnitudes(
            divide_signed_magnitude_exact(
                add_signed_magnitudes(value_1, value_minus_1),
                2
            ),
            value_0
        );
        coefficient_2 = subtract_signed_magnitudes(
            coefficient_2,
            value_infinity
        );

        SignedMagnitude weighted_odd = subtract_signed_magnitudes(
            value_2,
            value_0
        );
        weighted_odd = subtract_signed_magnitudes(
            weighted_odd,
            multiply_signed_magnitude_small(coefficient_2, 4)
        );
        weighted_odd = subtract_signed_magnitudes(
            weighted_odd,
            multiply_signed_magnitude_small(value_infinity, 16)
        );
        weighted_odd = divide_signed_magnitude_exact(
            std::move(weighted_odd),
            2
        );
        const SignedMagnitude coefficient_3 = divide_signed_magnitude_exact(
            subtract_signed_magnitudes(weighted_odd, odd_sum),
            3
        );
        const SignedMagnitude coefficient_1 = subtract_signed_magnitudes(
            odd_sum,
            coefficient_3
        );

        if(value_0.negative || coefficient_1.negative
            || coefficient_2.negative || coefficient_3.negative
            || value_infinity.negative){
            throw std::logic_error(
                "ExactInteger Toom-Cook interpolation became negative"
            );
        }
        std::vector<std::uint32_t> result;
        result.reserve(left.size() + right.size());
        add_shifted_magnitude(result, value_0.magnitude, 0);
        add_shifted_magnitude(result, coefficient_1.magnitude, split);
        add_shifted_magnitude(result, coefficient_2.magnitude, split * 2);
        add_shifted_magnitude(result, coefficient_3.magnitude, split * 3);
        add_shifted_magnitude(result, value_infinity.magnitude, split * 4);
        trim_magnitude(result);
        return result;
    }

    static std::vector<std::uint32_t> schoolbook_multiply(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        if(left.empty() || right.empty()) return {};
        std::vector<std::uint32_t> product(
            left.size() + right.size(), 0
        );
        for(std::size_t left_index = 0;
            left_index < left.size();
            ++left_index){
            std::uint64_t carry = 0;
            for(std::size_t right_index = 0;
                right_index < right.size();
                ++right_index){
                const std::size_t destination =
                    left_index + right_index;
                const std::uint64_t current =
                    static_cast<std::uint64_t>(left[left_index])
                        * right[right_index]
                    + product[destination] + carry;
                product[destination] =
                    static_cast<std::uint32_t>(current);
                carry = current >> 32;
            }
            product[left_index + right.size()] =
                static_cast<std::uint32_t>(carry);
        }
        trim_magnitude(product);
        return product;
    }

    static constexpr std::uint64_t goldilocks_modulus =
        18'446'744'069'414'584'321ULL;
    static constexpr std::uint64_t goldilocks_primitive_root = 7;
    static constexpr std::uint64_t goldilocks_maximum_transform_size =
        std::uint64_t{1} << 32;

#if defined(__SIZEOF_INT128__)
    static std::uint64_t goldilocks_reduce(__uint128_t value){
        constexpr std::uint64_t low_mask =
            (std::uint64_t{1} << 32) - 1;
        const std::uint64_t low = static_cast<std::uint64_t>(value);
        const std::uint64_t high = static_cast<std::uint64_t>(value >> 64);
        __uint128_t reduced = static_cast<__uint128_t>(low)
            + static_cast<__uint128_t>(high & low_mask) * low_mask
            + goldilocks_modulus - (high >> 32);
        if(reduced >= goldilocks_modulus) reduced -= goldilocks_modulus;
        if(reduced >= goldilocks_modulus) reduced -= goldilocks_modulus;
        return static_cast<std::uint64_t>(reduced);
    }
#else
#error "ExactInteger requires GCC 13 with unsigned 128-bit integers"
#endif

    static std::uint64_t goldilocks_multiply(
        std::uint64_t left,
        std::uint64_t right
    ){
        return goldilocks_reduce(static_cast<__uint128_t>(left) * right);
    }

    static std::uint64_t goldilocks_power(
        std::uint64_t base,
        std::uint64_t exponent
    ){
        std::uint64_t result = 1;
        while(exponent != 0){
            if((exponent & 1U) != 0){
                result = goldilocks_multiply(result, base);
            }
            base = goldilocks_multiply(base, base);
            exponent >>= 1;
        }
        return result;
    }

    static std::uint64_t goldilocks_add(
        std::uint64_t left,
        std::uint64_t right
    ){
        const std::uint64_t complement = goldilocks_modulus - right;
        return left >= complement ? left - complement : left + right;
    }

    static std::uint64_t goldilocks_subtract(
        std::uint64_t left,
        std::uint64_t right
    ){
        return left >= right
            ? left - right
            : goldilocks_modulus - (right - left);
    }

    static void goldilocks_transform(
        std::vector<std::uint64_t>& values,
        bool inverse
    ){
        const std::size_t size = values.size();
        for(std::size_t index = 1, reversed = 0; index < size; ++index){
            std::size_t bit = size >> 1;
            while((reversed & bit) != 0){
                reversed ^= bit;
                bit >>= 1;
            }
            reversed ^= bit;
            if(index < reversed) std::swap(values[index], values[reversed]);
        }
        for(std::size_t length = 2; length <= size; length <<= 1){
            std::uint64_t root = goldilocks_power(
                goldilocks_primitive_root,
                (goldilocks_modulus - 1) / length
            );
            if(inverse){
                root = goldilocks_power(root, goldilocks_modulus - 2);
            }
            const std::size_t half = length >> 1;
            for(std::size_t block = 0; block < size; block += length){
                std::uint64_t factor = 1;
                for(std::size_t offset = 0; offset < half; ++offset){
                    const std::uint64_t even = values[block + offset];
                    const std::uint64_t odd = goldilocks_multiply(
                        factor, values[block + offset + half]
                    );
                    values[block + offset] = goldilocks_add(even, odd);
                    values[block + offset + half] =
                        goldilocks_subtract(even, odd);
                    factor = goldilocks_multiply(factor, root);
                }
            }
            if(length == size) break;
        }
        if(inverse){
            const std::uint64_t inverse_size = goldilocks_power(
                static_cast<std::uint64_t>(size),
                goldilocks_modulus - 2
            );
            for(std::uint64_t& value: values){
                value = goldilocks_multiply(value, inverse_size);
            }
        }
    }

    static std::vector<std::uint32_t> limbs_to_ntt_digits(
        const std::vector<std::uint32_t>& limbs
    ){
        constexpr unsigned digit_bits = 15;
        constexpr std::uint64_t digit_mask =
            (std::uint64_t{1} << digit_bits) - 1;
        std::vector<std::uint32_t> digits;
        const __uint128_t digit_count =
            (static_cast<__uint128_t>(limbs.size()) * 32
                + digit_bits - 1) / digit_bits;
        if(digit_count > digits.max_size()){
            throw std::length_error("ExactInteger NTT input is too large");
        }
        digits.reserve(static_cast<std::size_t>(digit_count));
        std::uint64_t buffer = 0;
        unsigned buffered_bits = 0;
        for(const std::uint32_t limb: limbs){
            buffer |= static_cast<std::uint64_t>(limb) << buffered_bits;
            buffered_bits += 32;
            while(buffered_bits >= digit_bits){
                digits.push_back(static_cast<std::uint32_t>(
                    buffer & digit_mask
                ));
                buffer >>= digit_bits;
                buffered_bits -= digit_bits;
            }
        }
        if(buffered_bits != 0){
            digits.push_back(static_cast<std::uint32_t>(buffer));
        }
        while(!digits.empty() && digits.back() == 0) digits.pop_back();
        return digits;
    }

    static std::vector<std::uint64_t> goldilocks_convolution(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right,
        std::uint32_t maximum_digit
    ){
        if(left.empty() || right.empty()) return {};
        const std::size_t maximum_size =
            (std::numeric_limits<std::size_t>::max)();
        if(left.size() > maximum_size - (right.size() - 1)){
            throw std::length_error("ExactInteger convolution is too large");
        }
        const std::size_t coefficient_count =
            left.size() + right.size() - 1;
        if(coefficient_count > goldilocks_maximum_transform_size){
            throw std::length_error("ExactInteger NTT input is too large");
        }
        const std::uint64_t transform_size_64 = std::bit_ceil(
            static_cast<std::uint64_t>(coefficient_count)
        );
        std::vector<std::uint64_t> capacity_probe;
        if(transform_size_64 > capacity_probe.max_size()){
            throw std::length_error("ExactInteger NTT input is too large");
        }
#if defined(__SIZEOF_INT128__)
        const __uint128_t coefficient_bound =
            static_cast<__uint128_t>(std::min(left.size(), right.size()))
            * maximum_digit * maximum_digit;
        if(coefficient_bound >= goldilocks_modulus){
            throw std::length_error(
                "ExactInteger NTT coefficient is too large"
            );
        }
#endif
        const std::size_t transform_size =
            static_cast<std::size_t>(transform_size_64);
        std::vector<std::uint64_t> left_values(transform_size, 0);
        std::vector<std::uint64_t> right_values(transform_size, 0);
        std::copy(left.begin(), left.end(), left_values.begin());
        std::copy(right.begin(), right.end(), right_values.begin());
        goldilocks_transform(left_values, false);
        goldilocks_transform(right_values, false);
        for(std::size_t index = 0; index < transform_size; ++index){
            left_values[index] = goldilocks_multiply(
                left_values[index], right_values[index]
            );
        }
        goldilocks_transform(left_values, true);
        left_values.resize(coefficient_count);
        return left_values;
    }

    static std::vector<std::uint32_t> ntt_digits_to_limbs(
        const std::vector<std::uint64_t>& coefficients
    ){
        constexpr unsigned digit_bits = 15;
        constexpr std::uint64_t digit_mask =
            (std::uint64_t{1} << digit_bits) - 1;
        std::vector<std::uint32_t> digits;
        if(coefficients.size() > digits.max_size() - 4){
            throw std::length_error("ExactInteger NTT output is too large");
        }
        digits.reserve(coefficients.size() + 4);
        std::uint64_t carry = 0;
        for(const std::uint64_t coefficient: coefficients){
            const std::uint64_t current = coefficient + carry;
            digits.push_back(static_cast<std::uint32_t>(current & digit_mask));
            carry = current >> digit_bits;
        }
        while(carry != 0){
            digits.push_back(static_cast<std::uint32_t>(carry & digit_mask));
            carry >>= digit_bits;
        }
        std::vector<std::uint32_t> limbs;
        const __uint128_t limb_count =
            (static_cast<__uint128_t>(digits.size()) * digit_bits + 31) / 32;
        if(limb_count > limbs.max_size()){
            throw std::length_error("ExactInteger NTT output is too large");
        }
        limbs.reserve(static_cast<std::size_t>(limb_count));
        std::uint64_t buffer = 0;
        unsigned buffered_bits = 0;
        for(const std::uint32_t digit: digits){
            buffer |= static_cast<std::uint64_t>(digit) << buffered_bits;
            buffered_bits += digit_bits;
            if(buffered_bits >= 32){
                limbs.push_back(static_cast<std::uint32_t>(buffer));
                buffer >>= 32;
                buffered_bits -= 32;
            }
        }
        if(buffered_bits != 0){
            limbs.push_back(static_cast<std::uint32_t>(buffer));
        }
        trim_magnitude(limbs);
        return limbs;
    }

    static bool ntt_multiply_supported(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        if(left.empty() || right.empty()) return true;
#if defined(__SIZEOF_INT128__)
        const __uint128_t left_digits =
            (static_cast<__uint128_t>(left.size()) * 32 + 14) / 15;
        const __uint128_t right_digits =
            (static_cast<__uint128_t>(right.size()) * 32 + 14) / 15;
        return left_digits + right_digits - 1
            <= goldilocks_maximum_transform_size;
#else
        return false;
#endif
    }

    static std::vector<std::uint32_t> ntt_multiply(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        constexpr std::uint32_t maximum_digit =
            (std::uint32_t{1} << 15) - 1;
        const auto left_digits = limbs_to_ntt_digits(left);
        const auto right_digits = limbs_to_ntt_digits(right);
        return ntt_digits_to_limbs(goldilocks_convolution(
            left_digits, right_digits, maximum_digit
        ));
    }

    static std::vector<std::uint32_t> limb_range(
        const std::vector<std::uint32_t>& value,
        std::size_t first,
        std::size_t last
    ){
        first = std::min(first, value.size());
        last = std::min(last, value.size());
        if(first >= last) return {};
        std::vector<std::uint32_t> result(
            value.begin() + static_cast<std::ptrdiff_t>(first),
            value.begin() + static_cast<std::ptrdiff_t>(last)
        );
        trim_magnitude(result);
        return result;
    }

    static void add_shifted_magnitude(
        std::vector<std::uint32_t>& destination,
        const std::vector<std::uint32_t>& addition,
        std::size_t shift
    ){
        if(addition.empty()) return;
        const std::size_t required = shift + addition.size();
        if(destination.size() <= required){
            destination.resize(required + 1, 0);
        }
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < addition.size(); ++index){
            const std::size_t position = shift + index;
            const std::uint64_t sum =
                static_cast<std::uint64_t>(destination[position])
                + addition[index] + carry;
            destination[position] = static_cast<std::uint32_t>(sum);
            carry = sum >> 32;
        }
        std::size_t position = required;
        while(carry != 0){
            const std::uint64_t sum =
                static_cast<std::uint64_t>(destination[position]) + carry;
            destination[position] = static_cast<std::uint32_t>(sum);
            carry = sum >> 32;
            ++position;
            if(carry != 0 && position == destination.size()){
                destination.push_back(0);
            }
        }
        trim_magnitude(destination);
    }

    static std::vector<std::uint32_t> multiply_magnitudes(
        const std::vector<std::uint32_t>& left,
        const std::vector<std::uint32_t>& right
    ){
        if(left.empty() || right.empty()) return {};
        constexpr std::size_t karatsuba_threshold = 32;
        constexpr std::size_t toom_cook_3_threshold = 192;
        constexpr std::size_t ntt_threshold = 512;
        const std::size_t shorter = std::min(left.size(), right.size());
        const std::size_t longer = std::max(left.size(), right.size());
        const std::size_t toom_split =
            longer / 3 + (longer % 3 != 0);
        if(shorter <= karatsuba_threshold){
            return schoolbook_multiply(left, right);
        }

        // A single exact transform over the 64-bit Goldilocks prime handles
        // every practical allocation while keeping all coefficients unique.
        if(shorter >= ntt_threshold && ntt_multiply_supported(left, right)){
            return ntt_multiply(left, right);
        }

        // Three non-trivial chunks on both sides keep all five Toom-Cook
        // products useful.  More uneven inputs stay on the Karatsuba path.
        // At 192 limbs, the five smaller products amortize evaluation,
        // interpolation, and allocation without penalizing small values.
        if(shorter >= toom_cook_3_threshold
            && shorter > toom_split * 2){
            return toom_cook_3_multiply(left, right);
        }

        const std::size_t split = longer / 2;
        const auto left_low = limb_range(left, 0, split);
        const auto left_high = limb_range(left, split, left.size());
        const auto right_low = limb_range(right, 0, split);
        const auto right_high = limb_range(right, split, right.size());

        const auto low = multiply_magnitudes(left_low, right_low);
        const auto high = multiply_magnitudes(left_high, right_high);
        auto left_sum = add_magnitudes(left_low, left_high);
        auto right_sum = add_magnitudes(right_low, right_high);
        trim_magnitude(left_sum);
        trim_magnitude(right_sum);
        auto middle = multiply_magnitudes(left_sum, right_sum);
        middle = subtract_magnitudes(middle, low);
        trim_magnitude(middle);
        middle = subtract_magnitudes(middle, high);
        trim_magnitude(middle);

        std::vector<std::uint32_t> result;
        result.reserve(left.size() + right.size());
        add_shifted_magnitude(result, low, 0);
        add_shifted_magnitude(result, middle, split);
        add_shifted_magnitude(result, high, split * 2);
        trim_magnitude(result);

        return result;
    }

    static void trim_decimal(DecimalMagnitude& value){
        while(!value.empty() && value.back() == 0) value.pop_back();
    }

    static DecimalMagnitude add_decimals(
        const DecimalMagnitude& left,
        const DecimalMagnitude& right
    ){
        const std::size_t size = std::max(left.size(), right.size());
        DecimalMagnitude result;
        if(size == result.max_size()){
            throw std::length_error(
                "ExactInteger decimal conversion is too large"
            );
        }
        result.assign(size + 1, 0);
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < size; ++index){
            const std::uint64_t sum = carry
                + (index < left.size() ? left[index] : 0)
                + (index < right.size() ? right[index] : 0);
            result[index] = static_cast<std::uint32_t>(sum % decimal_base);
            carry = sum / decimal_base;
        }
        result[size] = static_cast<std::uint32_t>(carry);
        trim_decimal(result);
        return result;
    }

    static DecimalMagnitude subtract_decimals(
        const DecimalMagnitude& larger,
        const DecimalMagnitude& smaller
    ){
        DecimalMagnitude result(larger.size(), 0);
        std::uint64_t borrow = 0;
        for(std::size_t index = 0; index < larger.size(); ++index){
            const std::uint64_t subtrahend = borrow
                + (index < smaller.size() ? smaller[index] : 0);
            const std::uint64_t minuend = larger[index];
            if(minuend < subtrahend){
                result[index] = static_cast<std::uint32_t>(
                    minuend + decimal_base - subtrahend
                );
                borrow = 1;
            }else{
                result[index] = static_cast<std::uint32_t>(
                    minuend - subtrahend
                );
                borrow = 0;
            }
        }
        trim_decimal(result);
        return result;
    }

    static DecimalMagnitude schoolbook_multiply_decimals(
        const DecimalMagnitude& left,
        const DecimalMagnitude& right
    ){
        if(left.empty() || right.empty()) return {};
        DecimalMagnitude product;
        if(left.size() >= product.max_size()
            || right.size() > product.max_size() - left.size() - 1){
            throw std::length_error(
                "ExactInteger decimal conversion is too large"
            );
        }
        product.assign(left.size() + right.size() + 1, 0);
        for(std::size_t left_index = 0;
            left_index < left.size();
            ++left_index){
            std::uint64_t carry = 0;
            for(std::size_t right_index = 0;
                right_index < right.size();
                ++right_index){
                const std::size_t destination = left_index + right_index;
                const std::uint64_t current =
                    static_cast<std::uint64_t>(left[left_index])
                        * right[right_index]
                    + product[destination] + carry;
                product[destination] =
                    static_cast<std::uint32_t>(current % decimal_base);
                carry = current / decimal_base;
            }
            std::size_t destination = left_index + right.size();
            while(carry != 0){
                const std::uint64_t current =
                    static_cast<std::uint64_t>(product[destination]) + carry;
                product[destination] =
                    static_cast<std::uint32_t>(current % decimal_base);
                carry = current / decimal_base;
                ++destination;
            }
        }
        trim_decimal(product);
        return product;
    }

    static DecimalMagnitude decimal_range(
        const DecimalMagnitude& value,
        std::size_t first,
        std::size_t last
    ){
        first = std::min(first, value.size());
        last = std::min(last, value.size());
        if(first >= last) return {};
        return DecimalMagnitude(
            value.begin() + static_cast<std::ptrdiff_t>(first),
            value.begin() + static_cast<std::ptrdiff_t>(last)
        );
    }

    static void add_shifted_decimal(
        DecimalMagnitude& destination,
        const DecimalMagnitude& addition,
        std::size_t shift
    ){
        if(addition.empty()) return;
        if(shift >= destination.max_size()
            || addition.size() >= destination.max_size() - shift){
            throw std::length_error(
                "ExactInteger decimal conversion is too large"
            );
        }
        const std::size_t required = shift + addition.size();
        if(destination.size() <= required){
            destination.resize(required + 1, 0);
        }
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < addition.size(); ++index){
            const std::size_t position = shift + index;
            const std::uint64_t sum =
                static_cast<std::uint64_t>(destination[position])
                + addition[index] + carry;
            destination[position] =
                static_cast<std::uint32_t>(sum % decimal_base);
            carry = sum / decimal_base;
        }
        std::size_t position = required;
        while(carry != 0){
            const std::uint64_t sum =
                static_cast<std::uint64_t>(destination[position]) + carry;
            destination[position] =
                static_cast<std::uint32_t>(sum % decimal_base);
            carry = sum / decimal_base;
            ++position;
            if(carry != 0 && position == destination.size()){
                destination.push_back(0);
            }
        }
        trim_decimal(destination);
    }

    static std::vector<std::uint32_t> decimals_to_ntt_digits(
        const DecimalMagnitude& value
    ){
        constexpr std::uint32_t digit_base = 1'000;
        std::vector<std::uint32_t> digits;
        if(value.size() > digits.max_size() / 3){
            throw std::length_error(
                "ExactInteger decimal conversion is too large"
            );
        }
        digits.reserve(value.size() * 3);
        for(std::uint32_t chunk: value){
            digits.push_back(chunk % digit_base);
            chunk /= digit_base;
            digits.push_back(chunk % digit_base);
            digits.push_back(chunk / digit_base);
        }
        while(!digits.empty() && digits.back() == 0) digits.pop_back();
        return digits;
    }

    static DecimalMagnitude ntt_digits_to_decimals(
        const std::vector<std::uint64_t>& coefficients
    ){
        constexpr std::uint32_t digit_base = 1'000;
        std::vector<std::uint32_t> digits;
        if(coefficients.size() > digits.max_size() - 8){
            throw std::length_error(
                "ExactInteger decimal conversion is too large"
            );
        }
        digits.reserve(coefficients.size() + 8);
#if defined(__SIZEOF_INT128__)
        __uint128_t carry = 0;
        for(const std::uint64_t coefficient: coefficients){
            const __uint128_t current = coefficient + carry;
            digits.push_back(static_cast<std::uint32_t>(
                current % digit_base
            ));
            carry = current / digit_base;
        }
        while(carry != 0){
            digits.push_back(static_cast<std::uint32_t>(
                carry % digit_base
            ));
            carry /= digit_base;
        }
#endif
        DecimalMagnitude result;
        result.reserve(digits.size() / 3 + (digits.size() % 3 != 0));
        for(std::size_t index = 0; index < digits.size(); index += 3){
            std::uint32_t chunk = digits[index];
            if(index + 1 < digits.size()){
                chunk += digits[index + 1] * digit_base;
            }
            if(index + 2 < digits.size()){
                chunk += digits[index + 2] * digit_base * digit_base;
            }
            result.push_back(chunk);
        }
        trim_decimal(result);
        return result;
    }

    static DecimalMagnitude ntt_multiply_decimals(
        const DecimalMagnitude& left,
        const DecimalMagnitude& right
    ){
        constexpr std::uint32_t maximum_digit = 999;
        const auto left_digits = decimals_to_ntt_digits(left);
        const auto right_digits = decimals_to_ntt_digits(right);
        return ntt_digits_to_decimals(goldilocks_convolution(
            left_digits, right_digits, maximum_digit
        ));
    }

    static DecimalMagnitude multiply_decimals(
        const DecimalMagnitude& left,
        const DecimalMagnitude& right
    ){
        if(left.empty() || right.empty()) return {};
        constexpr std::size_t karatsuba_threshold = 32;
        constexpr std::size_t ntt_threshold = 256;
        const std::size_t shorter = std::min(left.size(), right.size());
        const std::size_t longer = std::max(left.size(), right.size());
        const bool ntt_supported =
            (static_cast<__uint128_t>(left.size()) + right.size()) * 3
                <= goldilocks_maximum_transform_size;
        if(shorter >= ntt_threshold && ntt_supported){
            return ntt_multiply_decimals(left, right);
        }
        if(shorter <= karatsuba_threshold || longer / shorter >= 2){
            return schoolbook_multiply_decimals(left, right);
        }

        const std::size_t split = longer / 2;
        const auto left_low = decimal_range(left, 0, split);
        const auto left_high = decimal_range(left, split, left.size());
        const auto right_low = decimal_range(right, 0, split);
        const auto right_high = decimal_range(right, split, right.size());

        const auto low = multiply_decimals(left_low, right_low);
        const auto high = multiply_decimals(left_high, right_high);
        const auto left_sum = add_decimals(left_low, left_high);
        const auto right_sum = add_decimals(right_low, right_high);
        auto middle = multiply_decimals(left_sum, right_sum);
        middle = subtract_decimals(middle, low);
        middle = subtract_decimals(middle, high);

        DecimalMagnitude result;
        add_shifted_decimal(result, low, 0);
        add_shifted_decimal(result, middle, split);
        add_shifted_decimal(result, high, split * 2);
        trim_decimal(result);
        return result;
    }

    DecimalMagnitude decimal_magnitude() const{
        std::vector<DecimalMagnitude> blocks;
        blocks.reserve(limbs_.size());
        for(const std::uint32_t limb: limbs_){
            DecimalMagnitude block{
                static_cast<std::uint32_t>(limb % decimal_base),
                static_cast<std::uint32_t>(limb / decimal_base)
            };
            trim_decimal(block);
            blocks.push_back(std::move(block));
        }

        DecimalMagnitude place_value{
            static_cast<std::uint32_t>(limb_base % decimal_base),
            static_cast<std::uint32_t>(limb_base / decimal_base)
        };
        while(blocks.size() > 1){
            std::vector<DecimalMagnitude> merged;
            merged.reserve(
                blocks.size() / 2 + blocks.size() % 2
            );
            for(std::size_t index = 0; index < blocks.size(); index += 2){
                if(index + 1 == blocks.size()){
                    merged.push_back(std::move(blocks[index]));
                    continue;
                }
                auto high = multiply_decimals(blocks[index + 1], place_value);
                merged.push_back(add_decimals(blocks[index], high));
            }
            blocks = std::move(merged);
            if(blocks.size() > 1){
                place_value = multiply_decimals(place_value, place_value);
            }
        }
        return blocks.empty() ? DecimalMagnitude{} : std::move(blocks.front());
    }

    void add_magnitude_one(){
        std::uint64_t carry = 1;
        for(std::uint32_t& limb: limbs_){
            const std::uint64_t sum = limb + carry;
            limb = static_cast<std::uint32_t>(sum);
            carry = sum >> 32;
            if(carry == 0) return;
        }
        if(carry != 0) limbs_.push_back(static_cast<std::uint32_t>(carry));
    }

    template<class Unsigned>
    Unsigned magnitude_to_unsigned() const{
        static_assert(!std::numeric_limits<Unsigned>::is_signed);
        Unsigned result = 0;
        if constexpr(std::numeric_limits<Unsigned>::digits <= 32){
            if(!limbs_.empty()) result = static_cast<Unsigned>(limbs_[0]);
        }else{
            for(std::size_t index = limbs_.size(); index-- > 0;){
                result = static_cast<Unsigned>(result << 32);
                result = static_cast<Unsigned>(result | limbs_[index]);
            }
        }
        return result;
    }

public:
    ExactInteger() = default;

    template<exact_integer_detail::NativeInteger Integer>
    ExactInteger(Integer value){
        assign_integral(value);
    }

    template<exact_integer_detail::NativeInteger Integer>
    ExactInteger& operator=(Integer value){
        assign_integral(value);
        return *this;
    }

    bool is_zero() const{
        return limbs_.empty();
    }

    bool is_negative() const{
        return negative_;
    }

    std::size_t bit_length() const{
        if(limbs_.empty()) return 0;
        const std::size_t complete_bits = (limbs_.size() - 1) * 32;
        return complete_bits + static_cast<std::size_t>(
            32 - std::countl_zero(limbs_.back())
        );
    }

    ExactInteger absolute() const{
        ExactInteger result = *this;
        result.negative_ = false;
        return result;
    }

    template<exact_integer_detail::NativeInteger Integer>
    Integer checked_to() const{
        using Value = std::remove_cv_t<Integer>;
        if constexpr(std::same_as<Value, bool>){
            if(*this == 0) return false;
            if(*this == 1) return true;
            throw std::overflow_error("ExactInteger does not fit target integer type");
        }else{
            const ExactInteger minimum = std::numeric_limits<Value>::is_signed
                ? ExactInteger((std::numeric_limits<Value>::min)())
                : ExactInteger(0);
            const ExactInteger maximum((std::numeric_limits<Value>::max)());
            if(*this < minimum || *this > maximum){
                throw std::overflow_error(
                    "ExactInteger does not fit target integer type"
                );
            }
            using Unsigned = exact_integer_detail::MakeUnsignedT<Value>;
            const Unsigned magnitude = magnitude_to_unsigned<Unsigned>();
            if constexpr(!std::numeric_limits<Value>::is_signed){
                return static_cast<Value>(magnitude);
            }else{
                if(!negative_) return static_cast<Value>(magnitude);
                const Unsigned minimum_magnitude =
                    static_cast<Unsigned>((std::numeric_limits<Value>::max)())
                    + Unsigned{1};
                if(magnitude == minimum_magnitude){
                    return (std::numeric_limits<Value>::min)();
                }
                return static_cast<Value>(-static_cast<Value>(magnitude));
            }
        }
    }

    std::pair<ExactInteger, std::uint64_t> divmod(
        std::uint64_t positive_divisor
    ) const{
        if(positive_divisor == 0){
            throw std::domain_error("ExactInteger division by zero");
        }
        ExactInteger quotient;
        quotient.limbs_.resize(limbs_.size(), 0);
        std::uint64_t remainder = 0;
        for(std::size_t index = limbs_.size(); index-- > 0;){
            std::uint32_t limb_quotient = 0;
            for(int bit = 31; bit >= 0; --bit){
                const std::uint64_t incoming =
                    (limbs_[index] >> bit) & std::uint32_t{1};
                const std::uint64_t half = positive_divisor / 2;
                const bool subtract = remainder > half
                    || (remainder == half
                        && (positive_divisor % 2 == 0 || incoming != 0));
                if(subtract){
                    if(positive_divisor % 2 == 0){
                        remainder = (remainder - half) * 2 + incoming;
                    }else if(remainder == half){
                        remainder = 0;
                    }else{
                        remainder = (remainder - half) * 2 - 1 + incoming;
                    }
                    limb_quotient |= std::uint32_t{1} << bit;
                }else{
                    remainder = remainder * 2 + incoming;
                }
            }
            quotient.limbs_[index] = limb_quotient;
        }
        quotient.negative_ = negative_;
        quotient.normalize();
        return {std::move(quotient), remainder};
    }

    ExactInteger operator-() const{
        ExactInteger result = *this;
        if(!result.is_zero()) result.negative_ = !result.negative_;
        return result;
    }

    ExactInteger& operator+=(const ExactInteger& other){
        if(other.is_zero()) return *this;
        if(is_zero()){
            *this = other;
            return *this;
        }
        if(negative_ == other.negative_){
            limbs_ = add_magnitudes(limbs_, other.limbs_);
        }else{
            const int order = compare_magnitude(*this, other);
            if(order == 0){
                limbs_.clear();
                negative_ = false;
                return *this;
            }
            if(order > 0){
                limbs_ = subtract_magnitudes(limbs_, other.limbs_);
            }else{
                limbs_ = subtract_magnitudes(other.limbs_, limbs_);
                negative_ = other.negative_;
            }
        }
        normalize();
        return *this;
    }

    template<exact_integer_detail::NativeInteger Integer>
    ExactInteger& operator+=(Integer value){
        return *this += ExactInteger(value);
    }

    ExactInteger& operator-=(const ExactInteger& other){
        return *this += -other;
    }

    template<exact_integer_detail::NativeInteger Integer>
    ExactInteger& operator-=(Integer value){
        return *this -= ExactInteger(value);
    }

    ExactInteger& operator*=(const ExactInteger& other){
        if(is_zero() || other.is_zero()){
            limbs_.clear();
            negative_ = false;
            return *this;
        }
        if(other.limbs_.size() > limbs_.max_size() - limbs_.size()){
            throw std::length_error("ExactInteger multiplication is too large");
        }
        auto product = multiply_magnitudes(limbs_, other.limbs_);
        negative_ = negative_ != other.negative_;
        limbs_ = std::move(product);
        normalize();
        return *this;
    }

    template<exact_integer_detail::NativeInteger Integer>
    ExactInteger& operator*=(Integer value){
        return *this *= ExactInteger(value);
    }

    ExactInteger& operator<<=(std::size_t shift){
        if(is_zero() || shift == 0) return *this;
        const std::size_t word_shift = shift / 32;
        const unsigned bit_shift = static_cast<unsigned>(shift % 32);
        if(limbs_.size() == limbs_.max_size()
            || word_shift > limbs_.max_size() - limbs_.size() - 1){
            throw std::length_error("ExactInteger left shift is too large");
        }
        std::vector<std::uint32_t> shifted(
            word_shift + limbs_.size() + 1, 0
        );
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < limbs_.size(); ++index){
            const std::uint64_t current =
                (static_cast<std::uint64_t>(limbs_[index]) << bit_shift)
                | carry;
            shifted[word_shift + index] = static_cast<std::uint32_t>(current);
            carry = current >> 32;
        }
        shifted[word_shift + limbs_.size()] =
            static_cast<std::uint32_t>(carry);
        limbs_ = std::move(shifted);
        normalize();
        return *this;
    }

    ExactInteger& operator>>=(std::size_t shift){
        if(is_zero() || shift == 0) return *this;
        const bool was_negative = negative_;
        const std::size_t word_shift = shift / 32;
        const unsigned bit_shift = static_cast<unsigned>(shift % 32);
        bool discarded = false;
        const std::size_t removed_words = std::min(word_shift, limbs_.size());
        for(std::size_t index = 0; index < removed_words; ++index){
            discarded = discarded || limbs_[index] != 0;
        }
        if(word_shift < limbs_.size() && bit_shift != 0){
            const std::uint32_t mask =
                (std::uint32_t{1} << bit_shift) - 1;
            discarded = discarded || (limbs_[word_shift] & mask) != 0;
        }
        if(word_shift >= limbs_.size()){
            limbs_.clear();
        }else{
            const std::size_t new_size = limbs_.size() - word_shift;
            std::vector<std::uint32_t> shifted(new_size, 0);
            for(std::size_t destination = 0; destination < new_size; ++destination){
                const std::size_t source = destination + word_shift;
                std::uint64_t value = limbs_[source] >> bit_shift;
                if(bit_shift != 0 && source + 1 < limbs_.size()){
                    value |= static_cast<std::uint64_t>(limbs_[source + 1])
                        << (32 - bit_shift);
                }
                shifted[destination] = static_cast<std::uint32_t>(value);
            }
            limbs_ = std::move(shifted);
        }
        normalize();
        if(was_negative && discarded){
            add_magnitude_one();
            negative_ = true;
        }
        return *this;
    }

    std::string to_string() const{
        if(is_zero()) return "0";
        const DecimalMagnitude chunks = decimal_magnitude();
        std::string result = negative_ ? "-" : "";
        const std::size_t sign_size = static_cast<std::size_t>(negative_);
        if(chunks.size() <= (result.max_size() - sign_size) / 9){
            result.reserve(chunks.size() * 9 + sign_size);
        }
        result += std::to_string(chunks.back());
        for(std::size_t index = chunks.size() - 1; index-- > 0;){
            std::string chunk = std::to_string(chunks[index]);
            result.append(9 - chunk.size(), '0');
            result += chunk;
        }
        return result;
    }

    friend ExactInteger abs(const ExactInteger& value){
        return value.absolute();
    }

    friend bool operator==(const ExactInteger& left, const ExactInteger& right){
        return left.negative_ == right.negative_ && left.limbs_ == right.limbs_;
    }

    friend std::strong_ordering operator<=> (
        const ExactInteger& left,
        const ExactInteger& right
    ){
        if(left.negative_ != right.negative_){
            return left.negative_ ? std::strong_ordering::less
                                  : std::strong_ordering::greater;
        }
        const int magnitude_order = compare_magnitude(left, right);
        if(magnitude_order == 0) return std::strong_ordering::equal;
        const bool less = left.negative_ ? magnitude_order > 0
                                         : magnitude_order < 0;
        return less ? std::strong_ordering::less
                    : std::strong_ordering::greater;
    }

    friend ExactInteger operator+(ExactInteger left, const ExactInteger& right){
        left += right;
        return left;
    }

    friend ExactInteger operator-(ExactInteger left, const ExactInteger& right){
        left -= right;
        return left;
    }

    friend ExactInteger operator*(ExactInteger left, const ExactInteger& right){
        left *= right;
        return left;
    }

    friend ExactInteger operator<<(ExactInteger value, std::size_t shift){
        value <<= shift;
        return value;
    }

    friend ExactInteger operator>>(ExactInteger value, std::size_t shift){
        value >>= shift;
        return value;
    }

    friend std::ostream& operator<<(std::ostream& stream, const ExactInteger& value){
        return stream << value.to_string();
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_INTEGER_EXACT_INTEGER_HPP_INCLUDED
