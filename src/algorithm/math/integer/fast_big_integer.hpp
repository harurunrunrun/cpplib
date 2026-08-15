#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTEGER_FAST_BIG_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTEGER_FAST_BIG_INTEGER_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(__SIZEOF_INT128__)
#error "BigInteger requires unsigned __int128."
#endif

namespace fast_big_integer_detail{

template<class Integer>
inline constexpr bool native_integer =
    std::is_integral_v<std::remove_cv_t<Integer>>
    || std::same_as<std::remove_cv_t<Integer>, __int128_t>
    || std::same_as<std::remove_cv_t<Integer>, __uint128_t>;

template<class Integer>
concept NativeInteger = native_integer<Integer>;

template<class Integer>
struct MakeUnsigned{
    using type = std::make_unsigned_t<Integer>;
};
template<>
struct MakeUnsigned<__int128_t>{using type = __uint128_t;};
template<>
struct MakeUnsigned<__uint128_t>{using type = __uint128_t;};
template<class Integer>
using MakeUnsignedT = typename MakeUnsigned<std::remove_cv_t<Integer>>::type;

constexpr std::uint64_t power_mod(
    std::uint64_t value,
    std::uint64_t exponent,
    std::uint64_t modulus
){
    std::uint64_t result = 1;
    while(exponent != 0){
        if((exponent & 1U) != 0) result = result * value % modulus;
        value = value * value % modulus;
        exponent >>= 1;
    }
    return result;
}

template<std::uint32_t Modulus>
class MontgomeryModInt{
    static_assert((Modulus & 1U) != 0);
    static_assert(Modulus < (std::uint32_t{1} << 30));

    static consteval std::uint32_t montgomery_inverse(){
        std::uint32_t inverse = Modulus;
        for(int iteration = 0; iteration < 5; ++iteration){
            inverse *= 2U - Modulus * inverse;
        }
        return 0U - inverse;
    }

    static constexpr std::uint32_t negative_inverse = montgomery_inverse();
    static constexpr std::uint32_t r_squared = static_cast<std::uint32_t>(
        (std::uint64_t{0} - Modulus) % Modulus
    );
    static constexpr std::uint32_t twice_modulus = Modulus * 2U;
    std::uint32_t value_ = 0;

    static constexpr std::uint32_t reduce(std::uint64_t value) noexcept{
        return static_cast<std::uint32_t>(
            (value
                + static_cast<std::uint64_t>(
                    static_cast<std::uint32_t>(value) * negative_inverse
                ) * Modulus) >> 32
        );
    }

public:
    constexpr MontgomeryModInt() = default;
    constexpr MontgomeryModInt(std::uint64_t value) noexcept:
        value_(reduce(value % Modulus * r_squared)){}

    constexpr MontgomeryModInt& operator+=(const MontgomeryModInt& other) noexcept{
        value_ += other.value_;
        if(value_ >= twice_modulus) value_ -= twice_modulus;
        return *this;
    }
    constexpr MontgomeryModInt& operator-=(const MontgomeryModInt& other) noexcept{
        if(value_ < other.value_) value_ += twice_modulus;
        value_ -= other.value_;
        return *this;
    }
    constexpr MontgomeryModInt& operator*=(const MontgomeryModInt& other) noexcept{
        value_ = reduce(static_cast<std::uint64_t>(value_) * other.value_);
        return *this;
    }
    friend constexpr MontgomeryModInt operator+(
        MontgomeryModInt left,
        const MontgomeryModInt& right
    ) noexcept{return left += right;}
    friend constexpr MontgomeryModInt operator-(
        MontgomeryModInt left,
        const MontgomeryModInt& right
    ) noexcept{return left -= right;}
    friend constexpr MontgomeryModInt operator*(
        MontgomeryModInt left,
        const MontgomeryModInt& right
    ) noexcept{return left *= right;}

    constexpr MontgomeryModInt pow(std::uint64_t exponent) const noexcept{
        MontgomeryModInt result(1), factor = *this;
        while(exponent != 0){
            if((exponent & 1U) != 0) result *= factor;
            factor *= factor;
            exponent >>= 1;
        }
        return result;
    }
    constexpr std::uint32_t value() const noexcept{
        std::uint32_t result = reduce(value_);
        if(result >= Modulus) result -= Modulus;
        return result;
    }
};

template<std::uint32_t Modulus, std::uint32_t PrimitiveRoot>
void radix_four_ntt(
    std::vector<MontgomeryModInt<Modulus>>& values,
    bool inverse
){
    using Mint = MontgomeryModInt<Modulus>;
    const std::size_t size = values.size();
    if(size <= 1) return;

    for(std::size_t index = 1, reversed = 0; index < size; ++index){
        std::size_t bit = size >> 1;
        while((reversed & bit) != 0){
            reversed ^= bit;
            bit >>= 1;
        }
        reversed ^= bit;
        if(index < reversed) std::swap(values[index], values[reversed]);
    }

    const unsigned levels = std::countr_zero(size);
    std::size_t first_size = 2;
    if((levels & 1U) != 0){
        for(std::size_t block = 0; block < size; block += 2){
            const Mint second = values[block + 1];
            values[block + 1] = values[block] - second;
            values[block] += second;
        }
        first_size = 4;
    }

    for(; first_size < size; first_size <<= 2){
        const std::size_t block_size = first_size << 1;
        const std::size_t quarter = first_size >> 1;
        Mint root = Mint(PrimitiveRoot).pow(
            (Modulus - 1) / static_cast<std::uint64_t>(block_size)
        );
        if(inverse) root = root.pow(Modulus - 2);
        const Mint imaginary = root.pow(quarter);

        for(std::size_t block = 0; block < size; block += block_size){
            Mint factor(1);
            for(std::size_t offset = 0; offset < quarter; ++offset){
                const Mint squared_factor = factor * factor;
                const Mint first = values[block + offset];
                const Mint second =
                    values[block + quarter + offset] * squared_factor;
                const Mint third = values[block + first_size + offset];
                const Mint fourth =
                    values[block + first_size + quarter + offset]
                    * squared_factor;
                const Mint low_sum = first + second;
                const Mint low_difference = first - second;
                const Mint high_sum = (third + fourth) * factor;
                const Mint high_difference =
                    (third - fourth) * imaginary * factor;
                values[block + offset] = low_sum + high_sum;
                values[block + first_size + offset] = low_sum - high_sum;
                values[block + quarter + offset] =
                    low_difference + high_difference;
                values[block + first_size + quarter + offset] =
                    low_difference - high_difference;
                factor *= root;
            }
        }
    }

    if(inverse){
        const Mint inverse_size = Mint(size).pow(Modulus - 2);
        for(Mint& value: values) value *= inverse_size;
    }
}

template<std::uint32_t Modulus, std::uint32_t PrimitiveRoot>
std::vector<std::uint32_t> convolution_mod(
    std::span<const std::uint32_t> left,
    std::span<const std::uint32_t> right,
    std::size_t transform_size,
    bool square
){
    using Mint = MontgomeryModInt<Modulus>;
    const std::size_t result_size = left.size() + right.size() - 1;
    std::vector<Mint> left_values(transform_size);
    for(std::size_t index = 0; index < left.size(); ++index){
        left_values[index] = Mint(left[index]);
    }
    radix_four_ntt<Modulus, PrimitiveRoot>(left_values, false);
    if(square){
        for(Mint& value: left_values) value *= value;
    }else{
        std::vector<Mint> right_values(transform_size);
        for(std::size_t index = 0; index < right.size(); ++index){
            right_values[index] = Mint(right[index]);
        }
        radix_four_ntt<Modulus, PrimitiveRoot>(right_values, false);
        for(std::size_t index = 0; index < transform_size; ++index){
            left_values[index] *= right_values[index];
        }
    }
    radix_four_ntt<Modulus, PrimitiveRoot>(left_values, true);
    std::vector<std::uint32_t> result(result_size);
    for(std::size_t index = 0; index < result_size; ++index){
        result[index] = left_values[index].value();
    }
    return result;
}

}  // namespace fast_big_integer_detail

class BigInteger{
    using Limb = std::uint32_t;
    using Wide = __uint128_t;
    static constexpr Limb limb_base = 1'000'000'000U;
    static constexpr std::size_t decimal_digits_per_limb = 9;
    static constexpr std::size_t inline_limb_capacity = 4;
    static constexpr std::size_t schoolbook_threshold = 128;
    static constexpr std::size_t division_basecase_threshold = 64;
    static constexpr std::size_t maximum_ntt_size = std::size_t{1} << 24;

    class LimbStorage{
        std::array<Limb, inline_limb_capacity> inline_values_{};
        std::vector<Limb> heap_values_;
        std::size_t inline_size_ = 0;
        bool heap_mode_ = false;
    public:
        LimbStorage() = default;
        LimbStorage(const LimbStorage&) = default;
        LimbStorage& operator=(const LimbStorage&) = default;
        LimbStorage(LimbStorage&& other) noexcept:
            inline_values_(other.inline_values_),
            heap_values_(std::move(other.heap_values_)),
            inline_size_(other.inline_size_),
            heap_mode_(other.heap_mode_){
            other.inline_size_ = 0;
            other.heap_mode_ = false;
            other.heap_values_.clear();
        }
        LimbStorage& operator=(LimbStorage&& other) noexcept{
            if(this == &other) return *this;
            inline_values_ = other.inline_values_;
            heap_values_ = std::move(other.heap_values_);
            inline_size_ = other.inline_size_;
            heap_mode_ = other.heap_mode_;
            other.inline_size_ = 0;
            other.heap_mode_ = false;
            other.heap_values_.clear();
            return *this;
        }
        std::size_t size() const noexcept{
            return heap_mode_ ? heap_values_.size() : inline_size_;
        }
        bool empty() const noexcept{return size() == 0;}
        Limb* data() noexcept{
            return heap_mode_ ? heap_values_.data() : inline_values_.data();
        }
        const Limb* data() const noexcept{
            return heap_mode_ ? heap_values_.data() : inline_values_.data();
        }
        Limb& operator[](std::size_t index) noexcept{return data()[index];}
        const Limb& operator[](std::size_t index) const noexcept{return data()[index];}
        Limb& back() noexcept{return data()[size() - 1];}
        const Limb& back() const noexcept{return data()[size() - 1];}
        Limb* begin() noexcept{return data();}
        Limb* end() noexcept{return data() + size();}
        const Limb* begin() const noexcept{return data();}
        const Limb* end() const noexcept{return data() + size();}
        void clear() noexcept{
            if(heap_mode_) heap_values_.clear();
            else inline_size_ = 0;
        }
        void resize(std::size_t size, Limb value = 0){
            if(heap_mode_){
                heap_values_.resize(size, value);
                return;
            }
            if(size <= inline_limb_capacity){
                if(size > inline_size_){
                    std::fill(
                        inline_values_.begin()
                            + static_cast<std::ptrdiff_t>(inline_size_),
                        inline_values_.begin()
                            + static_cast<std::ptrdiff_t>(size),
                        value
                    );
                }
                inline_size_ = size;
                return;
            }
            heap_values_.assign(size, value);
            std::copy_n(inline_values_.begin(), inline_size_, heap_values_.begin());
            heap_mode_ = true;
            inline_size_ = 0;
        }
        void push_back(Limb value){
            const std::size_t old_size = size();
            resize(old_size + 1);
            (*this)[old_size] = value;
        }
        void pop_back() noexcept{
            if(heap_mode_) heap_values_.pop_back();
            else --inline_size_;
        }
        void assign(std::vector<Limb>&& values){
            if(values.size() <= inline_limb_capacity){
                heap_values_.clear();
                heap_mode_ = false;
                inline_size_ = values.size();
                std::copy(values.begin(), values.end(), inline_values_.begin());
            }else{
                heap_values_ = std::move(values);
                heap_mode_ = true;
                inline_size_ = 0;
            }
        }
        std::span<const Limb> span() const noexcept{return {data(), size()};}
    };

    LimbStorage limbs_;
    bool negative_ = false;

    static void trim(std::vector<Limb>& value) noexcept{
        while(!value.empty() && value.back() == 0) value.pop_back();
    }
    void normalize() noexcept{
        while(!limbs_.empty() && limbs_.back() == 0) limbs_.pop_back();
        if(limbs_.empty()) negative_ = false;
    }
    static int compare_magnitudes(
        std::span<const Limb> left,
        std::span<const Limb> right
    ) noexcept{
        while(!left.empty() && left.back() == 0){
            left = left.first(left.size() - 1);
        }
        while(!right.empty() && right.back() == 0){
            right = right.first(right.size() - 1);
        }
        if(left.size() != right.size()) return left.size() < right.size() ? -1 : 1;
        for(std::size_t index = left.size(); index-- > 0;){
            if(left[index] != right[index]) return left[index] < right[index] ? -1 : 1;
        }
        return 0;
    }
    static std::vector<Limb> add_magnitudes(
        std::span<const Limb> left,
        std::span<const Limb> right
    ){
        const std::size_t size = std::max(left.size(), right.size());
        std::vector<Limb> result(size + 1, 0);
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < size; ++index){
            const std::uint64_t sum = carry
                + (index < left.size() ? left[index] : 0)
                + (index < right.size() ? right[index] : 0);
            if(sum >= limb_base){
                result[index] = static_cast<Limb>(sum - limb_base);
                carry = 1;
            }else{
                result[index] = static_cast<Limb>(sum);
                carry = 0;
            }
        }
        result[size] = static_cast<Limb>(carry);
        trim(result);
        return result;
    }
    static std::vector<Limb> subtract_magnitudes(
        std::span<const Limb> larger,
        std::span<const Limb> smaller
    ){
        std::vector<Limb> result(larger.size(), 0);
        std::uint64_t borrow = 0;
        for(std::size_t index = 0; index < larger.size(); ++index){
            const std::uint64_t subtrahend =
                (index < smaller.size() ? smaller[index] : 0) + borrow;
            if(larger[index] < subtrahend){
                result[index] = static_cast<Limb>(
                    static_cast<std::uint64_t>(larger[index])
                    + limb_base - subtrahend
                );
                borrow = 1;
            }else{
                result[index] = static_cast<Limb>(larger[index] - subtrahend);
                borrow = 0;
            }
        }
        trim(result);
        return result;
    }
    static std::vector<Limb> multiply_limb(
        std::span<const Limb> value,
        Limb multiplier
    ){
        if(value.empty() || multiplier == 0) return {};
        if(multiplier == 1) return {value.begin(), value.end()};
        std::vector<Limb> result(value.size() + 1, 0);
        std::uint64_t carry = 0;
        for(std::size_t index = 0; index < value.size(); ++index){
            const std::uint64_t product =
                static_cast<std::uint64_t>(value[index]) * multiplier + carry;
            result[index] = static_cast<Limb>(product % limb_base);
            carry = product / limb_base;
        }
        result[value.size()] = static_cast<Limb>(carry);
        trim(result);
        return result;
    }
    static std::pair<std::vector<Limb>, Limb> divide_limb(
        std::span<const Limb> value,
        Limb divisor
    ){
        std::vector<Limb> quotient(value.size(), 0);
        std::uint64_t remainder = 0;
        for(std::size_t index = value.size(); index-- > 0;){
            const std::uint64_t current = remainder * limb_base + value[index];
            quotient[index] = static_cast<Limb>(current / divisor);
            remainder = current % divisor;
        }
        trim(quotient);
        return {std::move(quotient), static_cast<Limb>(remainder)};
    }
    static std::vector<Limb> schoolbook_multiply(
        std::span<const Limb> left,
        std::span<const Limb> right
    ){
        if(left.size() < right.size()) std::swap(left, right);
        std::vector<Limb> result(left.size() + right.size(), 0);
        for(std::size_t right_index = 0; right_index < right.size(); ++right_index){
            std::uint64_t carry = 0;
            for(std::size_t left_index = 0; left_index < left.size(); ++left_index){
                const std::size_t result_index = left_index + right_index;
                const std::uint64_t current =
                    static_cast<std::uint64_t>(left[left_index]) * right[right_index]
                    + result[result_index] + carry;
                result[result_index] = static_cast<Limb>(current % limb_base);
                carry = current / limb_base;
            }
            result[right_index + left.size()] = static_cast<Limb>(carry);
        }
        trim(result);
        return result;
    }
    static std::vector<Limb> ntt_multiply(
        std::span<const Limb> left,
        std::span<const Limb> right,
        bool square
    ){
        constexpr std::uint32_t modulus0 = 167'772'161U;
        constexpr std::uint32_t modulus1 = 469'762'049U;
        constexpr std::uint32_t modulus2 = 754'974'721U;
        if(left.size() > (std::numeric_limits<std::size_t>::max)()
                - right.size() + 1){
            throw std::length_error("BigInteger multiplication is too large");
        }
        const std::size_t result_size = left.size() + right.size() - 1;
        if(result_size > maximum_ntt_size){
            throw std::length_error(std::string{});
        }
        const std::size_t transform_size = std::bit_ceil(result_size);
        auto residue0 = fast_big_integer_detail::convolution_mod<
            modulus0, 3U
        >(left, right, transform_size, square);
        auto residue1 = fast_big_integer_detail::convolution_mod<
            modulus1, 3U
        >(left, right, transform_size, square);
        auto residue2 = fast_big_integer_detail::convolution_mod<
            modulus2, 11U
        >(left, right, transform_size, square);
        constexpr std::uint64_t modulus01 =
            static_cast<std::uint64_t>(modulus0) * modulus1;
        constexpr std::uint64_t inverse0_mod1 =
            fast_big_integer_detail::power_mod(modulus0, modulus1 - 2, modulus1);
        constexpr std::uint64_t inverse01_mod2 =
            fast_big_integer_detail::power_mod(
                modulus01 % modulus2, modulus2 - 2, modulus2
            );
        std::vector<Limb> result;
        result.reserve(result_size + 3);
        Wide carry = 0;
        for(std::size_t index = 0; index < result_size; ++index){
            const std::uint64_t first = residue0[index];
            const std::uint64_t second_delta =
                (residue1[index] + modulus1 - first % modulus1) % modulus1;
            const std::uint64_t second =
                second_delta * inverse0_mod1 % modulus1;
            const std::uint64_t first_two = first + modulus0 * second;
            const std::uint64_t third_delta =
                (residue2[index] + modulus2 - first_two % modulus2) % modulus2;
            const std::uint64_t third =
                third_delta * inverse01_mod2 % modulus2;
            const Wide coefficient =
                static_cast<Wide>(first_two)
                + static_cast<Wide>(modulus01) * third;
            const Wide current = coefficient + carry;
            result.push_back(static_cast<Limb>(current % limb_base));
            carry = current / limb_base;
        }
        while(carry != 0){
            result.push_back(static_cast<Limb>(carry % limb_base));
            carry /= limb_base;
        }
        trim(result);
        return result;
    }
    static std::vector<Limb> multiply_magnitudes(
        std::span<const Limb> left,
        std::span<const Limb> right,
        bool square = false
    ){
        if(left.empty() || right.empty()) return {};
        if(left.size() == 1) return multiply_limb(right, left.front());
        if(right.size() == 1) return multiply_limb(left, right.front());
        if(std::min(left.size(), right.size()) <= schoolbook_threshold){
            return schoolbook_multiply(left, right);
        }
        return ntt_multiply(left, right, square);
    }
    static void decrement_magnitude(std::vector<Limb>& value){
        std::size_t index = 0;
        while(value[index] == 0){
            value[index] = limb_base - 1;
            ++index;
        }
        --value[index];
        trim(value);
    }
    static void increment_magnitude(std::vector<Limb>& value){
        std::size_t index = 0;
        while(index < value.size() && value[index] == limb_base - 1){
            value[index] = 0;
            ++index;
        }
        if(index == value.size()) value.push_back(1);
        else ++value[index];
    }
    static std::pair<std::vector<Limb>, std::vector<Limb>> long_division(
        std::span<const Limb> dividend,
        std::span<const Limb> divisor
    ){
        const int order = compare_magnitudes(dividend, divisor);
        if(order < 0) return {{}, {dividend.begin(), dividend.end()}};
        if(order == 0) return {{1}, {}};
        if(divisor.size() == 1){
            auto [quotient, remainder] = divide_limb(dividend, divisor[0]);
            return {
                std::move(quotient),
                remainder == 0 ? std::vector<Limb>{}
                               : std::vector<Limb>{remainder}
            };
        }
        const Limb normalization = static_cast<Limb>(
            limb_base / (static_cast<std::uint64_t>(divisor.back()) + 1)
        );
        std::vector<Limb> normalized_dividend =
            multiply_limb(dividend, normalization);
        std::vector<Limb> normalized_divisor =
            multiply_limb(divisor, normalization);
        normalized_dividend.resize(dividend.size() + 1, 0);
        const std::size_t divisor_size = normalized_divisor.size();
        const std::size_t quotient_size =
            dividend.size() - divisor.size() + 1;
        std::vector<Limb> quotient(quotient_size, 0);
        const std::uint64_t high_divisor = normalized_divisor.back();
        const std::uint64_t next_divisor =
            normalized_divisor[divisor_size - 2];

        for(std::size_t position = quotient_size; position-- > 0;){
            const std::uint64_t numerator =
                static_cast<std::uint64_t>(
                    normalized_dividend[position + divisor_size]
                ) * limb_base
                + normalized_dividend[position + divisor_size - 1];
            std::uint64_t estimate = numerator / high_divisor;
            std::uint64_t remainder = numerator % high_divisor;
            if(estimate >= limb_base){
                estimate = limb_base - 1;
                remainder = numerator - estimate * high_divisor;
            }
            while(remainder < limb_base
                && estimate * next_divisor
                    > remainder * limb_base
                        + normalized_dividend[position + divisor_size - 2]){
                --estimate;
                remainder += high_divisor;
            }

            std::uint64_t carry = 0;
            std::uint64_t borrow = 0;
            for(std::size_t index = 0; index < divisor_size; ++index){
                const std::uint64_t product =
                    estimate * normalized_divisor[index] + carry;
                carry = product / limb_base;
                const std::uint64_t subtrahend =
                    product % limb_base + borrow;
                Limb& current = normalized_dividend[position + index];
                if(current < subtrahend){
                    current = static_cast<Limb>(
                        static_cast<std::uint64_t>(current)
                        + limb_base - subtrahend
                    );
                    borrow = 1;
                }else{
                    current = static_cast<Limb>(current - subtrahend);
                    borrow = 0;
                }
            }
            Limb& high = normalized_dividend[position + divisor_size];
            const std::uint64_t high_subtrahend = carry + borrow;
            const bool overestimated = high < high_subtrahend;
            if(overestimated){
                high = static_cast<Limb>(
                    static_cast<std::uint64_t>(high)
                    + limb_base - high_subtrahend
                );
                --estimate;
                std::uint64_t addition_carry = 0;
                for(std::size_t index = 0; index < divisor_size; ++index){
                    const std::uint64_t sum =
                        static_cast<std::uint64_t>(
                            normalized_dividend[position + index]
                        ) + normalized_divisor[index] + addition_carry;
                    if(sum >= limb_base){
                        normalized_dividend[position + index] =
                            static_cast<Limb>(sum - limb_base);
                        addition_carry = 1;
                    }else{
                        normalized_dividend[position + index] =
                            static_cast<Limb>(sum);
                        addition_carry = 0;
                    }
                }
                const std::uint64_t corrected_high = high + addition_carry;
                high = static_cast<Limb>(
                    corrected_high >= limb_base
                        ? corrected_high - limb_base
                        : corrected_high
                );
            }else{
                high = static_cast<Limb>(high - high_subtrahend);
            }
            quotient[position] = static_cast<Limb>(estimate);
        }

        trim(quotient);
        std::vector<Limb> remainder(
            normalized_dividend.begin(),
            normalized_dividend.begin()
                + static_cast<std::ptrdiff_t>(divisor_size)
        );
        trim(remainder);
        if(normalization != 1){
            auto division = divide_limb(remainder, normalization);
            remainder = std::move(division.first);
        }
        return {std::move(quotient), std::move(remainder)};
    }
    static std::vector<Limb> reciprocal_approximation(
        const std::vector<Limb>& value,
        std::size_t precision
    ){
        std::size_t current_precision = precision;
        while(current_precision > division_basecase_threshold){
            current_precision = (current_precision + 1) / 2;
        }
        std::vector<Limb> numerator(
            value.size() + current_precision + 1, 0
        );
        numerator.back() = 1;
        std::vector<Limb> approximation =
            long_division(numerator, value).first;

        while(current_precision < precision){
            std::vector<Limb> square = multiply_magnitudes(
                approximation, approximation, true
            );
            square.insert(square.begin(), 0);
            const std::size_t window_size = current_precision * 2 + 1;
            std::vector<Limb> high_value(window_size, 0);
            const std::size_t copied = std::min(value.size(), window_size);
            std::copy(
                value.end() - static_cast<std::ptrdiff_t>(copied),
                value.end(),
                high_value.end() - static_cast<std::ptrdiff_t>(copied)
            );
            std::vector<Limb> correction = multiply_magnitudes(
                square, high_value
            );
            if(correction.size() <= window_size){
                correction.clear();
            }else{
                correction.erase(
                    correction.begin(),
                    correction.begin()
                        + static_cast<std::ptrdiff_t>(window_size)
                );
            }
            std::vector<Limb> doubled = add_magnitudes(
                approximation, approximation
            );
            std::vector<Limb> scaled(current_precision + 1, 0);
            scaled.insert(scaled.end(), doubled.begin(), doubled.end());
            approximation = subtract_magnitudes(scaled, correction);
            if(!approximation.empty()) approximation.erase(approximation.begin());
            trim(approximation);
            current_precision *= 2;
        }
        if(current_precision > precision){
            const std::size_t excess = current_precision - precision;
            approximation.erase(
                approximation.begin(),
                approximation.begin() + static_cast<std::ptrdiff_t>(excess)
            );
        }
        trim(approximation);
        return approximation;
    }
    static std::pair<std::vector<Limb>, std::vector<Limb>> newton_division(
        std::span<const Limb> dividend,
        std::span<const Limb> divisor
    ){
        const Limb normalization = static_cast<Limb>(
            limb_base / (static_cast<std::uint64_t>(divisor.back()) + 1)
        );
        std::vector<Limb> normalized_dividend =
            multiply_limb(dividend, normalization);
        std::vector<Limb> normalized_divisor =
            multiply_limb(divisor, normalization);
        const std::size_t precision =
            normalized_dividend.size() - normalized_divisor.size() + 2;
        std::vector<Limb> reciprocal = reciprocal_approximation(
            normalized_divisor, precision
        );
        std::vector<Limb> quotient = multiply_magnitudes(
            normalized_dividend, reciprocal
        );
        const std::size_t discarded = normalized_divisor.size() + precision;
        if(quotient.size() <= discarded){
            quotient.clear();
        }else{
            quotient.erase(
                quotient.begin(),
                quotient.begin() + static_cast<std::ptrdiff_t>(discarded)
            );
        }
        trim(quotient);
        std::vector<Limb> product = multiply_magnitudes(
            normalized_divisor, quotient
        );
        while(compare_magnitudes(normalized_dividend, product) < 0){
            decrement_magnitude(quotient);
            product = subtract_magnitudes(product, normalized_divisor);
        }
        std::vector<Limb> remainder = subtract_magnitudes(
            normalized_dividend, product
        );
        while(compare_magnitudes(remainder, normalized_divisor) >= 0){
            increment_magnitude(quotient);
            remainder = subtract_magnitudes(remainder, normalized_divisor);
        }
        if(normalization != 1){
            auto division = divide_limb(remainder, normalization);
            remainder = std::move(division.first);
        }
        trim(quotient);
        trim(remainder);
        return {std::move(quotient), std::move(remainder)};
    }
    static std::pair<std::vector<Limb>, std::vector<Limb>> divide_magnitudes(
        std::span<const Limb> dividend,
        std::span<const Limb> divisor
    ){
        const int order = compare_magnitudes(dividend, divisor);
        if(order < 0) return {{}, {dividend.begin(), dividend.end()}};
        if(order == 0) return {{1}, {}};
        if(divisor.size() == 1
            || divisor.size() <= division_basecase_threshold
            || dividend.size() - divisor.size()
                <= division_basecase_threshold){
            return long_division(dividend, divisor);
        }
        return newton_division(dividend, divisor);
    }
    template<fast_big_integer_detail::NativeInteger Integer>
    void assign_integral(Integer value){
        using Value = std::remove_cv_t<Integer>;
        limbs_.clear();
        negative_ = false;
        if constexpr(std::same_as<Value, bool>){
            if(value) limbs_.push_back(1);
        }else{
            using Unsigned = fast_big_integer_detail::MakeUnsignedT<Value>;
            Unsigned magnitude = static_cast<Unsigned>(value);
            if constexpr(std::numeric_limits<Value>::is_signed){
                if(value < 0){
                    negative_ = true;
                    magnitude = Unsigned{0} - magnitude;
                }
            }
            std::vector<Limb> limbs;
            while(magnitude != 0){
                limbs.push_back(static_cast<Limb>(magnitude % limb_base));
                magnitude /= limb_base;
            }
            limbs_.assign(std::move(limbs));
        }
    }
    static BigInteger from_magnitude(
        std::vector<Limb>&& magnitude,
        bool negative = false
    ){
        trim(magnitude);
        BigInteger result;
        result.negative_ = negative && !magnitude.empty();
        result.limbs_.assign(std::move(magnitude));
        return result;
    }

public:
    BigInteger() = default;
    template<fast_big_integer_detail::NativeInteger Integer>
    BigInteger(Integer value){assign_integral(value);}
    explicit BigInteger(std::string_view decimal){assign(decimal);}
    template<fast_big_integer_detail::NativeInteger Integer>
    BigInteger& operator=(Integer value){
        assign_integral(value);
        return *this;
    }
    BigInteger& assign(std::string_view decimal){
        if(decimal.empty()){
            throw std::invalid_argument("empty BigInteger literal");
        }
        bool negative = false;
        std::size_t begin = 0;
        if(decimal.front() == '+' || decimal.front() == '-'){
            negative = decimal.front() == '-';
            begin = 1;
        }
        if(begin == decimal.size()){
            throw std::invalid_argument("BigInteger literal has no digits");
        }
        for(std::size_t index = begin; index < decimal.size(); ++index){
            if(decimal[index] < '0' || decimal[index] > '9'){
                throw std::invalid_argument("invalid BigInteger decimal digit");
            }
        }
        while(begin < decimal.size() && decimal[begin] == '0') ++begin;
        if(begin == decimal.size()){
            limbs_.clear();
            negative_ = false;
            return *this;
        }
        std::vector<Limb> magnitude;
        magnitude.reserve(
            (decimal.size() - begin + decimal_digits_per_limb - 1)
            / decimal_digits_per_limb
        );
        std::size_t end = decimal.size();
        while(end > begin){
            const std::size_t block_begin =
                end - begin > decimal_digits_per_limb
                    ? end - decimal_digits_per_limb
                    : begin;
            Limb block = 0;
            for(std::size_t index = block_begin; index < end; ++index){
                block = static_cast<Limb>(
                    block * 10U + static_cast<unsigned>(decimal[index] - '0')
                );
            }
            magnitude.push_back(block);
            end = block_begin;
        }
        limbs_.assign(std::move(magnitude));
        negative_ = negative;
        return *this;
    }
    bool is_zero() const noexcept{return limbs_.empty();}
    bool is_negative() const noexcept{return negative_;}
    BigInteger absolute() const{
        BigInteger result = *this;
        result.negative_ = false;
        return result;
    }
    std::string to_string() const{
        if(is_zero()) return "0";
        char highest_buffer[16];
        const auto [highest_end, error] = std::to_chars(
            highest_buffer,
            highest_buffer + sizeof(highest_buffer),
            limbs_.back()
        );
        if(error != std::errc{}){
            throw std::runtime_error("BigInteger decimal conversion failed");
        }
        const std::size_t highest_size = static_cast<std::size_t>(
            highest_end - highest_buffer
        );
        std::string result;
        result.reserve(
            static_cast<std::size_t>(negative_)
            + highest_size
            + (limbs_.size() - 1) * decimal_digits_per_limb
        );
        if(negative_) result.push_back('-');
        result.append(highest_buffer, highest_end);
        for(std::size_t index = limbs_.size() - 1; index-- > 0;){
            char block[decimal_digits_per_limb];
            Limb value = limbs_[index];
            for(std::size_t position = decimal_digits_per_limb; position-- > 0;){
                block[position] = static_cast<char>('0' + value % 10U);
                value /= 10U;
            }
            result.append(block, block + decimal_digits_per_limb);
        }
        return result;
    }
    template<fast_big_integer_detail::NativeInteger Integer>
    Integer checked_to() const{
        using Value = std::remove_cv_t<Integer>;
        if constexpr(std::same_as<Value, bool>){
            if(*this == 0) return false;
            if(*this == 1) return true;
            throw std::overflow_error(
                "BigInteger does not fit target integer type"
            );
        }else{
            using Unsigned = fast_big_integer_detail::MakeUnsignedT<Value>;
            Unsigned limit;
            if constexpr(std::numeric_limits<Value>::is_signed){
                const Unsigned positive_limit = static_cast<Unsigned>(
                    (std::numeric_limits<Value>::max)()
                );
                limit = negative_ ? positive_limit + Unsigned{1}
                                  : positive_limit;
            }else{
                if(negative_){
                    throw std::overflow_error(
                        "BigInteger does not fit target integer type"
                    );
                }
                limit = (std::numeric_limits<Value>::max)();
            }
            Unsigned magnitude = 0;
            for(std::size_t index = limbs_.size(); index-- > 0;){
                const Unsigned digit = static_cast<Unsigned>(limbs_[index]);
                if(digit > limit
                    || magnitude > (limit - digit) / limb_base){
                    throw std::overflow_error(
                        "BigInteger does not fit target integer type"
                    );
                }
                magnitude = magnitude * limb_base + digit;
            }
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
    static std::pair<BigInteger, BigInteger> divmod(
        const BigInteger& dividend,
        const BigInteger& divisor
    ){
        if(divisor.is_zero()){
            throw std::domain_error("BigInteger division by zero");
        }
        auto [quotient, remainder] = divide_magnitudes(
            dividend.limbs_.span(), divisor.limbs_.span()
        );
        return {
            from_magnitude(
                std::move(quotient),
                dividend.negative_ != divisor.negative_
            ),
            from_magnitude(std::move(remainder), dividend.negative_)
        };
    }
    BigInteger operator-() const{
        BigInteger result = *this;
        if(!result.is_zero()) result.negative_ = !result.negative_;
        return result;
    }
    BigInteger& operator+=(const BigInteger& other){
        if(other.is_zero()) return *this;
        if(is_zero()){
            *this = other;
            return *this;
        }
        if(negative_ == other.negative_){
            limbs_.assign(add_magnitudes(limbs_.span(), other.limbs_.span()));
            return *this;
        }
        const int order = compare_magnitudes(
            limbs_.span(), other.limbs_.span()
        );
        if(order == 0){
            limbs_.clear();
            negative_ = false;
        }else if(order > 0){
            limbs_.assign(subtract_magnitudes(
                limbs_.span(), other.limbs_.span()
            ));
        }else{
            limbs_.assign(subtract_magnitudes(
                other.limbs_.span(), limbs_.span()
            ));
            negative_ = other.negative_;
        }
        normalize();
        return *this;
    }
    BigInteger& operator-=(const BigInteger& other){
        if(this == &other){
            limbs_.clear();
            negative_ = false;
            return *this;
        }
        BigInteger negated = -other;
        return *this += negated;
    }
    BigInteger& operator*=(const BigInteger& other){
        const bool result_negative = negative_ != other.negative_;
        const bool square = this == &other;
        std::vector<Limb> product = multiply_magnitudes(
            limbs_.span(), other.limbs_.span(), square
        );
        limbs_.assign(std::move(product));
        negative_ = result_negative && !limbs_.empty();
        return *this;
    }
    BigInteger& operator/=(const BigInteger& other){
        *this = divmod(*this, other).first;
        return *this;
    }
    BigInteger& operator%=(const BigInteger& other){
        *this = divmod(*this, other).second;
        return *this;
    }
    BigInteger& operator++(){
        *this += 1;
        return *this;
    }
    BigInteger operator++(int){
        BigInteger result = *this;
        ++*this;
        return result;
    }
    BigInteger& operator--(){
        *this -= 1;
        return *this;
    }
    BigInteger operator--(int){
        BigInteger result = *this;
        --*this;
        return result;
    }
    friend BigInteger abs(const BigInteger& value){return value.absolute();}
    friend bool operator==(
        const BigInteger& left,
        const BigInteger& right
    ){
        if(left.negative_ != right.negative_
            || left.limbs_.size() != right.limbs_.size()){
            return false;
        }
        return std::equal(
            left.limbs_.begin(),
            left.limbs_.end(),
            right.limbs_.begin()
        );
    }
    friend std::strong_ordering operator<=> (
        const BigInteger& left,
        const BigInteger& right
    ){
        if(left.negative_ != right.negative_){
            return left.negative_ ? std::strong_ordering::less
                                  : std::strong_ordering::greater;
        }
        const int order = compare_magnitudes(
            left.limbs_.span(), right.limbs_.span()
        );
        if(order == 0) return std::strong_ordering::equal;
        const bool less = left.negative_ ? order > 0 : order < 0;
        return less ? std::strong_ordering::less
                    : std::strong_ordering::greater;
    }
    friend BigInteger operator+(
        BigInteger left,
        const BigInteger& right
    ){return left += right;}
    friend BigInteger operator-(
        BigInteger left,
        const BigInteger& right
    ){return left -= right;}
    friend BigInteger operator*(
        const BigInteger& left,
        const BigInteger& right
    ){
        return from_magnitude(
            multiply_magnitudes(
                left.limbs_.span(),
                right.limbs_.span(),
                &left == &right
            ),
            left.negative_ != right.negative_
        );
    }
    friend BigInteger operator/(
        BigInteger left,
        const BigInteger& right
    ){return left /= right;}
    friend BigInteger operator%(
        BigInteger left,
        const BigInteger& right
    ){return left %= right;}
    friend std::ostream& operator<<(
        std::ostream& stream,
        const BigInteger& value
    ){return stream << value.to_string();}
    friend std::istream& operator>>(
        std::istream& stream,
        BigInteger& value
    ){
        std::string text;
        if(!(stream >> text)) return stream;
        try{
            BigInteger parsed(text);
            value = std::move(parsed);
        }catch(const std::invalid_argument&){
            stream.setstate(std::ios::failbit);
        }
        return stream;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_INTEGER_FAST_BIG_INTEGER_HPP_INCLUDED
