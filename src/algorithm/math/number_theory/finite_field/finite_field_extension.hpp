#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FINITE_FIELD_FINITE_FIELD_EXTENSION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FINITE_FIELD_FINITE_FIELD_EXTENSION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace number_theory {

template<std::uint64_t Prime>
class FiniteFieldExtension {
    static_assert(Prime >= 2, "base field characteristic must be at least 2");

public:
    using Element = std::vector<std::uint64_t>;

private:
    Element modulus_;
    std::size_t degree_;

    static std::uint64_t add_coefficient(
        std::uint64_t first,
        std::uint64_t second
    ) {
        return static_cast<std::uint64_t>(
            (static_cast<unsigned __int128>(first)
             + static_cast<unsigned __int128>(second))
            % Prime
        );
    }

    static std::uint64_t subtract_coefficient(
        std::uint64_t first,
        std::uint64_t second
    ) {
        return first >= second
            ? first - second
            : static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(first) + Prime - second
            );
    }

    static std::uint64_t multiply_coefficient(
        std::uint64_t first,
        std::uint64_t second
    ) {
        return static_cast<std::uint64_t>(
            static_cast<unsigned __int128>(first) * second % Prime
        );
    }

    static std::uint64_t coefficient_power(
        std::uint64_t base,
        std::uint64_t exponent
    ) {
        std::uint64_t result = 1 % Prime;
        base %= Prime;
        while(exponent != 0){
            if(exponent & 1U){
                result = multiply_coefficient(result, base);
            }
            base = multiply_coefficient(base, base);
            exponent >>= 1;
        }
        return result;
    }

    static void trim(Element& polynomial) {
        while(!polynomial.empty() && polynomial.back() == 0){
            polynomial.pop_back();
        }
    }

    static Element polynomial_add(
        const Element& first,
        const Element& second
    ) {
        Element result(std::max(first.size(), second.size()));
        for(std::size_t index = 0; index < result.size(); ++index){
            result[index] = add_coefficient(
                index < first.size() ? first[index] : 0,
                index < second.size() ? second[index] : 0
            );
        }
        trim(result);
        return result;
    }

    static Element polynomial_subtract(
        const Element& first,
        const Element& second
    ) {
        Element result(std::max(first.size(), second.size()));
        for(std::size_t index = 0; index < result.size(); ++index){
            result[index] = subtract_coefficient(
                index < first.size() ? first[index] : 0,
                index < second.size() ? second[index] : 0
            );
        }
        trim(result);
        return result;
    }

    static Element polynomial_multiply(
        const Element& first,
        const Element& second
    ) {
        if(first.empty() || second.empty()) return {};
        Element result(first.size() + second.size() - 1);
        for(std::size_t i = 0; i < first.size(); ++i){
            for(std::size_t j = 0; j < second.size(); ++j){
                result[i + j] = add_coefficient(
                    result[i + j],
                    multiply_coefficient(first[i], second[j])
                );
            }
        }
        trim(result);
        return result;
    }

    static std::pair<Element, Element> polynomial_divide(
        Element dividend,
        const Element& divisor
    ) {
        if(divisor.empty()){
            throw std::invalid_argument("polynomial division by zero");
        }
        trim(dividend);
        if(dividend.size() < divisor.size()) return {{}, dividend};
        Element quotient(dividend.size() - divisor.size() + 1);
        const std::uint64_t inverse_leading =
            coefficient_power(divisor.back(), Prime - 2);
        while(dividend.size() >= divisor.size()){
            const std::size_t shift = dividend.size() - divisor.size();
            const std::uint64_t factor =
                multiply_coefficient(dividend.back(), inverse_leading);
            quotient[shift] = factor;
            for(std::size_t index = 0; index < divisor.size(); ++index){
                dividend[index + shift] = subtract_coefficient(
                    dividend[index + shift],
                    multiply_coefficient(factor, divisor[index])
                );
            }
            trim(dividend);
        }
        trim(quotient);
        return {std::move(quotient), std::move(dividend)};
    }

public:
    explicit FiniteFieldExtension(Element modulus)
        : modulus_(std::move(modulus)),
          degree_(modulus_.empty() ? 0 : modulus_.size() - 1) {
        for(std::uint64_t& coefficient : modulus_) coefficient %= Prime;
        trim(modulus_);
        if(modulus_.size() < 2 || modulus_.back() != 1){
            throw std::invalid_argument(
                "extension modulus must be a monic polynomial of positive degree"
            );
        }
        degree_ = modulus_.size() - 1;
    }

    [[nodiscard]] std::size_t degree() const noexcept {
        return degree_;
    }

    [[nodiscard]] const Element& modulus() const noexcept {
        return modulus_;
    }

    [[nodiscard]] Element normalize(Element value) const {
        for(std::uint64_t& coefficient : value) coefficient %= Prime;
        trim(value);
        while(value.size() > degree_){
            const std::size_t shift = value.size() - modulus_.size();
            const std::uint64_t factor = value.back();
            for(std::size_t index = 0; index < modulus_.size(); ++index){
                value[index + shift] = subtract_coefficient(
                    value[index + shift],
                    multiply_coefficient(factor, modulus_[index])
                );
            }
            trim(value);
        }
        value.resize(degree_);
        return value;
    }

    [[nodiscard]] Element zero() const {
        return Element(degree_);
    }

    [[nodiscard]] Element one() const {
        Element result(degree_);
        result[0] = 1 % Prime;
        return result;
    }

    [[nodiscard]] Element add(
        const Element& first,
        const Element& second
    ) const {
        return normalize(polynomial_add(first, second));
    }

    [[nodiscard]] Element subtract(
        const Element& first,
        const Element& second
    ) const {
        return normalize(polynomial_subtract(first, second));
    }

    [[nodiscard]] Element multiply(
        const Element& first,
        const Element& second
    ) const {
        return normalize(polynomial_multiply(first, second));
    }

    [[nodiscard]] Element power(Element base, std::uint64_t exponent) const {
        base = normalize(std::move(base));
        Element result = one();
        while(exponent != 0){
            if(exponent & 1U) result = multiply(result, base);
            base = multiply(base, base);
            exponent >>= 1;
        }
        return result;
    }

    [[nodiscard]] Element inverse(Element value) const {
        value = normalize(std::move(value));
        trim(value);
        if(value.empty()){
            throw std::invalid_argument("zero has no multiplicative inverse");
        }
        Element previous = modulus_;
        Element current = value;
        Element previous_coefficient{};
        Element current_coefficient{1};
        while(!current.empty()){
            auto [quotient, remainder] =
                polynomial_divide(previous, current);
            Element next_coefficient = polynomial_subtract(
                previous_coefficient,
                polynomial_multiply(quotient, current_coefficient)
            );
            previous = std::move(current);
            current = std::move(remainder);
            previous_coefficient = std::move(current_coefficient);
            current_coefficient = std::move(next_coefficient);
        }
        if(previous.size() != 1){
            throw std::invalid_argument(
                "element is not invertible modulo the supplied polynomial"
            );
        }
        const std::uint64_t inverse_gcd =
            coefficient_power(previous[0], Prime - 2);
        for(std::uint64_t& coefficient : previous_coefficient){
            coefficient = multiply_coefficient(
                coefficient, inverse_gcd
            );
        }
        return normalize(std::move(previous_coefficient));
    }

    [[nodiscard]] Element divide(
        const Element& numerator,
        const Element& denominator
    ) const {
        return multiply(numerator, inverse(denominator));
    }
};

}  // namespace number_theory

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FINITE_FIELD_FINITE_FIELD_EXTENSION_HPP_INCLUDED
