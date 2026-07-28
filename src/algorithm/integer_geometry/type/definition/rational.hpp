#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPE_DEFINITION_RATIONAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPE_DEFINITION_RATIONAL_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <compare>
#include <concepts>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../../math/integer/exact_integer.hpp"

namespace integer_geometry{

using Integer = ExactInteger;

namespace detail{

inline std::pair<Integer, Integer> nonnegative_divmod(
    Integer dividend,
    const Integer& divisor
){
    if(dividend < 0 || divisor <= 0){
        throw std::invalid_argument(
            "nonnegative_divmod requires a nonnegative dividend "
            "and a positive divisor"
        );
    }
    Integer quotient = 0;
    if(dividend < divisor) return {std::move(quotient), std::move(dividend)};

    std::size_t shift = dividend.bit_length() - divisor.bit_length();
    Integer shifted_divisor = divisor << shift;
    Integer quotient_bit = Integer{1} << shift;
    while(true){
        if(shifted_divisor <= dividend){
            dividend -= shifted_divisor;
            quotient += quotient_bit;
        }
        if(shift == 0) break;
        shifted_divisor >>= 1;
        quotient_bit >>= 1;
        --shift;
    }
    return {std::move(quotient), std::move(dividend)};
}

inline bool halve_if_even(Integer& value){
    auto quotient_remainder = value.divmod(2);
    if(quotient_remainder.second != 0) return false;
    value = std::move(quotient_remainder.first);
    return true;
}

inline Integer gcd(Integer first, Integer second){
    first = abs(first);
    second = abs(second);
    if(first.is_zero()) return second;
    if(second.is_zero()) return first;
    if(first == 1 || second == 1) return Integer{1};

    std::size_t common_power_of_two = 0;
    while(true){
        auto first_half = first.divmod(2);
        auto second_half = second.divmod(2);
        if(first_half.second != 0 || second_half.second != 0) break;
        first = std::move(first_half.first);
        second = std::move(second_half.first);
        ++common_power_of_two;
    }
    while(halve_if_even(first)){}
    while(!second.is_zero()){
        while(halve_if_even(second)){}
        if(first > second) std::swap(first, second);
        second -= first;
    }
    first <<= common_power_of_two;
    return first;
}

inline Integer divide_exact(const Integer& dividend, const Integer& divisor){
    if(divisor.is_zero()){
        throw std::domain_error("exact integer division by zero");
    }
    if(divisor == 1) return dividend;
    if(divisor == -1) return -dividend;
    const bool negative = dividend.is_negative() != divisor.is_negative();
    const Integer magnitude_dividend = abs(dividend);
    const Integer magnitude_divisor = abs(divisor);
    if(magnitude_divisor.bit_length() <= 64){
        const std::uint64_t native_divisor =
            magnitude_divisor.checked_to<std::uint64_t>();
        auto quotient_remainder = magnitude_dividend.divmod(native_divisor);
        if(quotient_remainder.second != 0){
            throw std::logic_error("integer division is not exact");
        }
        return negative ? -quotient_remainder.first
                        : quotient_remainder.first;
    }
    auto quotient_remainder =
        nonnegative_divmod(magnitude_dividend, magnitude_divisor);
    if(!quotient_remainder.second.is_zero()){
        throw std::logic_error("integer division is not exact");
    }
    return negative ? -quotient_remainder.first : quotient_remainder.first;
}

}  // namespace detail

class Rational{
    Integer numerator_ = 0;
    Integer denominator_ = 1;

    void normalize(){
        if(denominator_.is_zero()){
            throw std::invalid_argument("rational denominator must be nonzero");
        }
        if(numerator_.is_zero()){
            denominator_ = 1;
            return;
        }
        if(denominator_.is_negative()){
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        if(denominator_ == 1) return;
        const Integer divisor = detail::gcd(abs(numerator_), denominator_);
        numerator_ = detail::divide_exact(numerator_, divisor);
        denominator_ = detail::divide_exact(denominator_, divisor);
    }

public:
    Rational() = default;

    template<exact_integer_detail::NativeInteger Native>
    Rational(Native value) : numerator_(value){}

    Rational(Integer numerator, Integer denominator = Integer{1})
        : numerator_(std::move(numerator)),
          denominator_(std::move(denominator)){
        normalize();
    }

    const Integer& numerator() const noexcept{
        return numerator_;
    }

    const Integer& denominator() const noexcept{
        return denominator_;
    }

    bool is_zero() const noexcept{
        return numerator_.is_zero();
    }

    int sign() const noexcept{
        return (numerator_ > 0) - (numerator_ < 0);
    }

    Rational reciprocal() const{
        if(is_zero()) throw std::domain_error("rational division by zero");
        return Rational(denominator_, numerator_);
    }

    Rational operator+() const{
        return *this;
    }

    Rational operator-() const{
        return Rational(-numerator_, denominator_);
    }

    Rational& operator+=(const Rational& other){
        const Integer common = detail::gcd(denominator_, other.denominator_);
        const Integer left_factor =
            detail::divide_exact(other.denominator_, common);
        const Integer right_factor =
            detail::divide_exact(denominator_, common);
        numerator_ = numerator_ * left_factor
            + other.numerator_ * right_factor;
        denominator_ *= left_factor;
        normalize();
        return *this;
    }

    Rational& operator-=(const Rational& other){
        return *this += -other;
    }

    Rational& operator*=(const Rational& other){
        const Integer first_divisor =
            detail::gcd(abs(numerator_), other.denominator_);
        const Integer second_divisor =
            detail::gcd(abs(other.numerator_), denominator_);
        numerator_ = detail::divide_exact(numerator_, first_divisor)
            * detail::divide_exact(other.numerator_, second_divisor);
        denominator_ = detail::divide_exact(denominator_, second_divisor)
            * detail::divide_exact(other.denominator_, first_divisor);
        return *this;
    }

    Rational& operator/=(const Rational& other){
        if(other.is_zero()) throw std::domain_error("rational division by zero");
        return *this *= other.reciprocal();
    }

    friend Rational operator+(Rational left, const Rational& right){
        left += right;
        return left;
    }

    friend Rational operator-(Rational left, const Rational& right){
        left -= right;
        return left;
    }

    friend Rational operator*(Rational left, const Rational& right){
        left *= right;
        return left;
    }

    friend Rational operator/(Rational left, const Rational& right){
        left /= right;
        return left;
    }

    friend bool operator==(const Rational& left, const Rational& right){
        return left.numerator_ == right.numerator_
            && left.denominator_ == right.denominator_;
    }

    friend std::strong_ordering operator<=> (
        const Rational& left,
        const Rational& right
    ){
        return left.numerator_ * right.denominator_
            <=> right.numerator_ * left.denominator_;
    }

    std::string to_string() const{
        if(denominator_ == 1) return numerator_.to_string();
        return numerator_.to_string() + "/" + denominator_.to_string();
    }

    friend Rational abs(const Rational& value){
        return Rational(abs(value.numerator_), value.denominator_);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Rational& value){
        return stream << value.to_string();
    }
};

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPE_DEFINITION_RATIONAL_HPP_INCLUDED
