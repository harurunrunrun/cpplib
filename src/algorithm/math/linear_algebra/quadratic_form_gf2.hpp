#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_QUADRATIC_FORM_GF2_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_QUADRATIC_FORM_GF2_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../integer/big_integer.hpp"

namespace math{

struct QuadraticFormGF2Canonical{
    std::size_t variable_count = 0;
    bool constant = false;
    std::vector<std::array<bool, 2>> symplectic_linear;
    std::vector<bool> radical_linear;
    std::vector<std::vector<std::uint64_t>> canonical_basis;

    std::size_t bilinear_rank() const noexcept{
        return 2 * symplectic_linear.size();
    }

    bool is_balanced() const noexcept{
        for(const bool coefficient: radical_linear){
            if(coefficient) return true;
        }
        return false;
    }

    std::optional<bool> arf_invariant() const noexcept{
        if(is_balanced()) return std::nullopt;
        bool result = false;
        for(const auto& coefficients: symplectic_linear){
            result ^= coefficients[0] && coefficients[1];
        }
        return result;
    }

    bool evaluate_canonical(const std::vector<bool>& assignment) const{
        if(assignment.size() != variable_count)[[unlikely]]{
            throw std::invalid_argument(
                "canonical quadratic-form assignment has the wrong size"
            );
        }
        bool result = constant;
        std::size_t coordinate = 0;
        for(const auto& coefficients: symplectic_linear){
            const bool first = assignment[coordinate++];
            const bool second = assignment[coordinate++];
            result ^= first && second;
            result ^= coefficients[0] && first;
            result ^= coefficients[1] && second;
        }
        for(const bool coefficient: radical_linear){
            result ^= coefficient && assignment[coordinate];
            ++coordinate;
        }
        return result;
    }

    std::vector<bool> to_original_assignment(
        const std::vector<bool>& canonical_assignment
    ) const{
        if(canonical_assignment.size() != variable_count)[[unlikely]]{
            throw std::invalid_argument(
                "canonical quadratic-form assignment has the wrong size"
            );
        }
        const std::size_t word_count =
            (variable_count + 63) / 64;
        std::vector<std::uint64_t> packed(word_count);
        for(std::size_t coordinate = 0;
            coordinate < variable_count; ++coordinate){
            if(!canonical_assignment[coordinate]) continue;
            for(std::size_t word = 0; word < word_count; ++word){
                packed[word] ^= canonical_basis[coordinate][word];
            }
        }
        std::vector<bool> result(variable_count);
        for(std::size_t index = 0; index < variable_count; ++index){
            result[index] =
                ((packed[index >> 6] >> (index & 63)) & 1U) != 0;
        }
        return result;
    }

    BigInteger solution_count(const bool value) const{
        if(variable_count == 0){
            return BigInteger(value == constant ? 1 : 0);
        }
        const BigInteger half = BigInteger(1) << (variable_count - 1);
        if(is_balanced()) return half;
        const BigInteger difference = BigInteger(1) <<
            (variable_count - symplectic_linear.size() - 1);
        bool negative_walsh = constant;
        for(const auto& coefficients: symplectic_linear){
            negative_walsh ^= coefficients[0] && coefficients[1];
        }
        const BigInteger zero_count = negative_walsh
            ? half - difference
            : half + difference;
        return value ? (BigInteger(1) << variable_count) - zero_count
                     : zero_count;
    }
};

class QuadraticFormGF2{
private:
    std::size_t variable_count_ = 0;
    std::size_t word_count_ = 0;
    bool constant_ = false;
    std::vector<std::uint64_t> linear_;
    std::vector<std::vector<std::uint64_t>> quadratic_;

    static bool bit(
        const std::vector<std::uint64_t>& values,
        const std::size_t index
    ) noexcept{
        return ((values[index >> 6] >> (index & 63)) & 1U) != 0;
    }

    static void toggle_bit(
        std::vector<std::uint64_t>& values,
        const std::size_t index
    ) noexcept{
        values[index >> 6] ^= std::uint64_t{1} << (index & 63);
    }

    void check_index(const std::size_t index) const{
        if(index >= variable_count_)[[unlikely]]{
            throw std::out_of_range("quadratic-form variable index is out of range");
        }
    }

public:
    explicit QuadraticFormGF2(const std::size_t variable_count)
        : variable_count_(variable_count),
          word_count_((variable_count + 63) / 64),
          linear_(word_count_),
          quadratic_(
              variable_count,
              std::vector<std::uint64_t>(word_count_)
          ){}

    std::size_t variable_count() const noexcept{ return variable_count_; }
    bool constant() const noexcept{ return constant_; }
    void set_constant(const bool value) noexcept{ constant_ = value; }
    void toggle_constant() noexcept{ constant_ = !constant_; }

    bool linear_coefficient(const std::size_t index) const{
        check_index(index);
        return bit(linear_, index);
    }

    void set_linear(const std::size_t index, const bool value){
        check_index(index);
        if(bit(linear_, index) != value) toggle_bit(linear_, index);
    }

    void toggle_linear(const std::size_t index){
        check_index(index);
        toggle_bit(linear_, index);
    }

    bool quadratic_coefficient(
        std::size_t first,
        std::size_t second
    ) const{
        check_index(first);
        check_index(second);
        if(first == second) return bit(linear_, first);
        if(first > second) std::swap(first, second);
        return bit(quadratic_[first], second);
    }

    void set_quadratic(
        std::size_t first,
        std::size_t second,
        const bool value
    ){
        check_index(first);
        check_index(second);
        if(first == second){
            set_linear(first, value);
            return;
        }
        if(first > second) std::swap(first, second);
        if(bit(quadratic_[first], second) != value){
            toggle_bit(quadratic_[first], second);
        }
    }

    void toggle_quadratic(std::size_t first, std::size_t second){
        check_index(first);
        check_index(second);
        if(first == second){
            toggle_bit(linear_, first);
            return;
        }
        if(first > second) std::swap(first, second);
        toggle_bit(quadratic_[first], second);
    }

    bool evaluate(const std::vector<bool>& assignment) const{
        if(assignment.size() != variable_count_)[[unlikely]]{
            throw std::invalid_argument(
                "quadratic-form assignment has the wrong size"
            );
        }
        bool result = constant_;
        for(std::size_t index = 0; index < variable_count_; ++index){
            result ^= bit(linear_, index) && assignment[index];
            if(!assignment[index]) continue;
            for(std::size_t second = index + 1;
                second < variable_count_; ++second){
                result ^= bit(quadratic_[index], second) &&
                    assignment[second];
            }
        }
        return result;
    }

    QuadraticFormGF2Canonical canonicalize() const{
        std::vector<std::vector<std::uint64_t>> polar(
            variable_count_, std::vector<std::uint64_t>(word_count_)
        );
        for(std::size_t first = 0; first < variable_count_; ++first){
            for(std::size_t word = 0; word < word_count_; ++word){
                std::uint64_t remaining = quadratic_[first][word];
                while(remaining != 0){
                    const unsigned offset = static_cast<unsigned>(
                        __builtin_ctzll(remaining)
                    );
                    const std::size_t second = word * 64 + offset;
                    if(second < variable_count_){
                        toggle_bit(polar[first], second);
                        toggle_bit(polar[second], first);
                    }
                    remaining &= remaining - 1;
                }
            }
        }

        std::vector<std::vector<std::uint64_t>> basis(
            variable_count_, std::vector<std::uint64_t>(word_count_)
        );
        std::vector<bool> form_value(variable_count_);
        for(std::size_t index = 0; index < variable_count_; ++index){
            toggle_bit(basis[index], index);
            form_value[index] = bit(linear_, index);
        }

        const auto swap_coordinates = [&](const std::size_t first,
                                          const std::size_t second){
            if(first == second) return;
            std::swap(basis[first], basis[second]);
            const bool saved = form_value[first];
            form_value[first] = form_value[second];
            form_value[second] = saved;
            std::swap(polar[first], polar[second]);
            for(std::size_t row = 0; row < variable_count_; ++row){
                const bool first_bit = bit(polar[row], first);
                const bool second_bit = bit(polar[row], second);
                if(first_bit != second_bit){
                    toggle_bit(polar[row], first);
                    toggle_bit(polar[row], second);
                }
            }
        };

        std::size_t processed = 0;
        while(processed + 1 < variable_count_){
            std::size_t first_pivot = variable_count_;
            std::size_t second_pivot = variable_count_;
            for(std::size_t row = processed;
                row < variable_count_ && first_pivot == variable_count_;
                ++row){
                for(std::size_t word = processed >> 6;
                    word < word_count_; ++word){
                    std::uint64_t candidates = polar[row][word];
                    if(word == (processed >> 6)){
                        candidates &= ~std::uint64_t{0} << (processed & 63);
                    }
                    if(candidates == 0) continue;
                    first_pivot = row;
                    second_pivot = word * 64 + static_cast<std::size_t>(
                        __builtin_ctzll(candidates)
                    );
                    break;
                }
            }
            if(first_pivot == variable_count_) break;
            swap_coordinates(processed, first_pivot);
            if(second_pivot == processed){
                second_pivot = first_pivot;
            }else if(second_pivot == first_pivot){
                second_pivot = processed;
            }
            swap_coordinates(processed + 1, second_pivot);

            std::vector<std::uint64_t> pair_with_second(word_count_);
            std::vector<std::uint64_t> pair_with_first(word_count_);
            for(std::size_t index = processed + 2;
                index < variable_count_; ++index){
                if(bit(polar[index], processed + 1)){
                    toggle_bit(pair_with_second, index);
                }
                if(bit(polar[index], processed)){
                    toggle_bit(pair_with_first, index);
                }
            }
            for(std::size_t index = processed + 2;
                index < variable_count_; ++index){
                const bool add_first = bit(pair_with_second, index);
                const bool add_second = bit(pair_with_first, index);
                if(add_first){
                    for(std::size_t word = 0; word < word_count_; ++word){
                        basis[index][word] ^= basis[processed][word];
                    }
                }
                if(add_second){
                    for(std::size_t word = 0; word < word_count_; ++word){
                        basis[index][word] ^= basis[processed + 1][word];
                    }
                }
                form_value[index] = form_value[index] ^
                    (add_first && form_value[processed]) ^
                    (add_second && form_value[processed + 1]) ^
                    (add_first && add_second);
            }
            for(std::size_t row = processed + 2;
                row < variable_count_; ++row){
                if(bit(pair_with_second, row)){
                    for(std::size_t word = 0; word < word_count_; ++word){
                        polar[row][word] ^= pair_with_first[word];
                    }
                }
                if(bit(pair_with_first, row)){
                    for(std::size_t word = 0; word < word_count_; ++word){
                        polar[row][word] ^= pair_with_second[word];
                    }
                }
                if(bit(polar[row], processed)){
                    toggle_bit(polar[row], processed);
                }
                if(bit(polar[row], processed + 1)){
                    toggle_bit(polar[row], processed + 1);
                }
            }
            for(std::size_t index = processed + 2;
                index < variable_count_; ++index){
                if(bit(polar[processed], index)){
                    toggle_bit(polar[processed], index);
                }
                if(bit(polar[processed + 1], index)){
                    toggle_bit(polar[processed + 1], index);
                }
            }
            processed += 2;
        }

        QuadraticFormGF2Canonical result;
        result.variable_count = variable_count_;
        result.constant = constant_;
        result.canonical_basis = std::move(basis);
        result.symplectic_linear.reserve(processed / 2);
        for(std::size_t index = 0; index < processed; index += 2){
            result.symplectic_linear.push_back(
                {form_value[index], form_value[index + 1]}
            );
        }
        result.radical_linear.reserve(variable_count_ - processed);
        for(std::size_t index = processed; index < variable_count_; ++index){
            result.radical_linear.push_back(form_value[index]);
        }
        return result;
    }
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_QUADRATIC_FORM_GF2_HPP_INCLUDED
