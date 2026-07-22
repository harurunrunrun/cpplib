#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_HPP_INCLUDED

#include <compare>
#include <concepts>
#include <cstddef>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

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
        ExactInteger result = 0;
        for(; position < text.size(); ++position){
            const char character = text[position];
            if(character < '0' || character > '9'){
                throw std::invalid_argument("invalid BigInteger decimal digit");
            }
            result *= 10;
            result += character - '0';
        }
        return negative ? -result : result;
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
        if(remainder >= positive_divisor){
            const std::size_t shift =
                remainder.bit_length() - positive_divisor.bit_length();
            ExactInteger shifted_divisor = positive_divisor << shift;
            ExactInteger quotient_bit = ExactInteger(1) << shift;
            while(true){
                if(remainder >= shifted_divisor){
                    remainder -= shifted_divisor;
                    quotient += quotient_bit;
                }
                if(quotient_bit == 1) break;
                shifted_divisor >>= 1;
                quotient_bit >>= 1;
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
