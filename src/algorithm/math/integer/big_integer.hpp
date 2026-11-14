#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_HPP_INCLUDED

#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "exact_integer.hpp"

class BigInteger{
private:
    ExactInteger value_;

    static ExactInteger parse_decimal(std::string_view text){
        if(text.empty()){
            throw std::invalid_argument("empty BigInteger literal");
        }
        bool negative = false;
        std::size_t position = 0;
        if(text.front() == '+' || text.front() == '-'){
            negative = text.front() == '-';
            position = 1;
        }
        if(position == text.size()){
            throw std::invalid_argument("BigInteger literal has no digits");
        }
        for(std::size_t index = position; index < text.size(); ++index){
            const char character = text[index];
            if(character < '0' || character > '9'){
                throw std::invalid_argument("invalid BigInteger decimal digit");
            }
        }
        ExactInteger result = parse_digits(text.substr(position));
        return negative ? -result : result;
    }

    static constexpr std::size_t chunk_digits = 18;
    static constexpr std::size_t base_case_digits = chunk_digits * 32;

    // digits consists of '0'..'9' only.  Reads chunk_digits digits per multiplication.
    static ExactInteger parse_digits_simple(std::string_view digits){
        ExactInteger result = 0;
        std::size_t position = 0;
        std::size_t length = digits.size() % chunk_digits;
        if(length == 0) length = chunk_digits;
        while(position < digits.size()){
            std::uint64_t chunk = 0;
            std::uint64_t scale = 1;
            for(std::size_t index = 0; index < length; ++index){
                chunk = chunk * 10 + static_cast<std::uint64_t>(digits[position + index] - '0');
                scale *= 10;
            }
            if(!result.is_zero()) result *= scale;
            result += chunk;
            position += length;
            length = chunk_digits;
        }
        return result;
    }

    // powers[k] = 10^(base_case_digits * 2^k).
    static ExactInteger parse_digits_recursive(
        std::string_view digits,
        const std::vector<ExactInteger>& powers
    ){
        if(digits.size() <= base_case_digits) return parse_digits_simple(digits);
        std::size_t level = 0;
        while((base_case_digits << (level + 1)) < digits.size()) ++level;
        const std::size_t low_size = base_case_digits << level;
        ExactInteger result = parse_digits_recursive(
            digits.substr(0, digits.size() - low_size), powers
        );
        result *= powers[level];
        result += parse_digits_recursive(digits.substr(digits.size() - low_size), powers);
        return result;
    }

    static ExactInteger parse_digits(std::string_view digits){
        if(digits.size() <= base_case_digits) return parse_digits_simple(digits);
        std::vector<ExactInteger> powers;
        powers.push_back(parse_digits_simple("1" + std::string(base_case_digits, '0')));
        while((base_case_digits << powers.size()) < digits.size()){
            powers.push_back(powers.back() * powers.back());
        }
        return parse_digits_recursive(digits, powers);
    }

public:
    BigInteger() = default;

    template<exact_integer_detail::NativeInteger Integer>
    BigInteger(Integer value): value_(value){}

    explicit BigInteger(std::string_view decimal): value_(parse_decimal(decimal)){}

    explicit BigInteger(const ExactInteger& value): value_(value){}
    explicit BigInteger(ExactInteger&& value): value_(std::move(value)){}

    template<exact_integer_detail::NativeInteger Integer>
    BigInteger& operator=(Integer value){
        value_ = value;
        return *this;
    }

    BigInteger& assign(std::string_view decimal){
        value_ = parse_decimal(decimal);
        return *this;
    }

    bool is_zero() const{
        return value_.is_zero();
    }

    bool is_negative() const{
        return value_.is_negative();
    }

    std::size_t bit_length() const{
        return value_.bit_length();
    }

    BigInteger absolute() const{
        return BigInteger(value_.absolute());
    }

    std::string to_string() const{
        return value_.to_string();
    }

    template<exact_integer_detail::NativeInteger Integer>
    Integer checked_to() const{
        return value_.template checked_to<Integer>();
    }

    const ExactInteger& exact_integer() const noexcept{
        return value_;
    }

    static std::pair<BigInteger, BigInteger> divmod(
        const BigInteger& dividend,
        const BigInteger& divisor
    ){
        if(divisor.is_zero()){
            throw std::domain_error("BigInteger division by zero");
        }
        ExactInteger remainder = dividend.value_.absolute();
        ExactInteger positive_divisor = divisor.value_.absolute();
        ExactInteger quotient = 0;
        const std::size_t divisor_bits = positive_divisor.bit_length();
        if(remainder < positive_divisor){
        }else if(divisor_bits <= 64){
            auto [magnitude_quotient, magnitude_remainder] =
                remainder.divmod(positive_divisor.checked_to<std::uint64_t>());
            quotient = std::move(magnitude_quotient);
            remainder = magnitude_remainder;
        }else{
            // Long division producing about 31 quotient bits per step.  The top 32 bits of
            // the divisor, rounded up, give an underestimate of each quotient digit.
            const std::size_t divisor_shift = divisor_bits - 32;
            const std::uint64_t divisor_top =
                (positive_divisor >> divisor_shift).checked_to<std::uint64_t>() + 1;
            while(remainder >= positive_divisor){
                const std::size_t remainder_bits = remainder.bit_length();
                const std::size_t shift = remainder_bits - divisor_shift > 64
                    ? remainder_bits - divisor_shift - 64 : 0;
                const std::uint64_t remainder_top =
                    (remainder >> (divisor_shift + shift)).checked_to<std::uint64_t>();
                std::uint64_t digit = remainder_top / divisor_top;
                if(digit == 0) digit = 1;
                ExactInteger product = positive_divisor;
                product *= digit;
                product <<= shift;
                remainder -= product;
                ExactInteger quotient_digit(digit);
                quotient_digit <<= shift;
                quotient += quotient_digit;
            }
        }
        if(dividend.is_negative() != divisor.is_negative()) quotient = -quotient;
        if(dividend.is_negative()) remainder = -remainder;
        return {
            BigInteger(std::move(quotient)),
            BigInteger(std::move(remainder))
        };
    }

    BigInteger operator-() const{
        return BigInteger(-value_);
    }

    BigInteger& operator+=(const BigInteger& other){
        value_ += other.value_;
        return *this;
    }

    BigInteger& operator-=(const BigInteger& other){
        value_ -= other.value_;
        return *this;
    }

    BigInteger& operator*=(const BigInteger& other){
        value_ *= other.value_;
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

    BigInteger& operator<<=(std::size_t shift){
        value_ <<= shift;
        return *this;
    }

    BigInteger& operator>>=(std::size_t shift){
        value_ >>= shift;
        return *this;
    }

    BigInteger& operator++(){
        value_ += 1;
        return *this;
    }

    BigInteger operator++(int){
        BigInteger result = *this;
        ++*this;
        return result;
    }

    BigInteger& operator--(){
        value_ -= 1;
        return *this;
    }

    BigInteger operator--(int){
        BigInteger result = *this;
        --*this;
        return result;
    }

    friend BigInteger abs(const BigInteger& value){
        return value.absolute();
    }

    friend bool operator==(const BigInteger&, const BigInteger&) = default;

    friend std::strong_ordering operator<=> (
        const BigInteger& left,
        const BigInteger& right
    ){
        return left.value_ <=> right.value_;
    }

    friend BigInteger operator+(BigInteger left, const BigInteger& right){
        left += right;
        return left;
    }

    friend BigInteger operator-(BigInteger left, const BigInteger& right){
        left -= right;
        return left;
    }

    friend BigInteger operator*(BigInteger left, const BigInteger& right){
        left *= right;
        return left;
    }

    friend BigInteger operator/(BigInteger left, const BigInteger& right){
        left /= right;
        return left;
    }

    friend BigInteger operator%(BigInteger left, const BigInteger& right){
        left %= right;
        return left;
    }

    friend BigInteger operator<<(BigInteger value, std::size_t shift){
        value <<= shift;
        return value;
    }

    friend BigInteger operator>>(BigInteger value, std::size_t shift){
        value >>= shift;
        return value;
    }

    friend std::ostream& operator<<(std::ostream& stream, const BigInteger& value){
        return stream << value.to_string();
    }

    friend std::istream& operator>>(std::istream& stream, BigInteger& value){
        std::string token;
        if(!(stream >> token)) return stream;
        try{
            value.assign(token);
        }catch(const std::invalid_argument&){
            stream.setstate(std::ios::failbit);
        }
        return stream;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_HPP_INCLUDED
