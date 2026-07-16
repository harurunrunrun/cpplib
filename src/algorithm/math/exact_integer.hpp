#pragma once

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

    static std::vector<std::uint32_t> limb_range(
        const std::vector<std::uint32_t>& value,
        std::size_t first,
        std::size_t last
    ){
        first = std::min(first, value.size());
        last = std::min(last, value.size());
        if(first >= last) return {};
        return std::vector<std::uint32_t>(
            value.begin() + static_cast<std::ptrdiff_t>(first),
            value.begin() + static_cast<std::ptrdiff_t>(last)
        );
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
        if(std::min(left.size(), right.size()) <= karatsuba_threshold){
            return schoolbook_multiply(left, right);
        }

        const std::size_t split =
            std::max(left.size(), right.size()) / 2;
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

    static DecimalMagnitude multiply_decimals(
        const DecimalMagnitude& left,
        const DecimalMagnitude& right
    ){
        if(left.empty() || right.empty()) return {};
        constexpr std::size_t karatsuba_threshold = 32;
        const std::size_t shorter = std::min(left.size(), right.size());
        const std::size_t longer = std::max(left.size(), right.size());
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
        return complete_bits + (32 - std::countl_zero(limbs_.back()));
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
