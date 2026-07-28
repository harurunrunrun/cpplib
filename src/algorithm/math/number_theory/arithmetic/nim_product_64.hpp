#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_NIM_PRODUCT_64_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_NIM_PRODUCT_64_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace math{
namespace nim_product_internal{

constexpr std::uint64_t recursive_product(
    std::uint64_t left,
    std::uint64_t right,
    unsigned int bits
){
    if(bits == 1) return left & right;
    const unsigned int half = bits / 2;
    const std::uint64_t mask = (std::uint64_t{1} << half) - 1;
    const std::uint64_t left_low = left & mask;
    const std::uint64_t left_high = left >> half;
    const std::uint64_t right_low = right & mask;
    const std::uint64_t right_high = right >> half;
    const std::uint64_t high_product =
        recursive_product(left_high, right_high, half);
    const std::uint64_t low_product =
        recursive_product(left_low, right_low, half);
    const std::uint64_t mixed_product = recursive_product(
        left_low ^ left_high, right_low ^ right_high, half
    );
    const std::uint64_t twist = recursive_product(
        high_product, std::uint64_t{1} << (half - 1), half
    );
    return ((mixed_product ^ low_product) << half)
        ^ twist ^ low_product;
}

class ByteProductTable{
    static constexpr std::size_t byte_values = 256;
    static constexpr std::size_t byte_pairs = 64;
    static constexpr std::size_t pair_stride =
        byte_values * byte_values;

    std::vector<std::uint64_t> products_;

    static constexpr std::size_t index(
        unsigned int left_byte,
        unsigned int right_byte,
        unsigned int left_value,
        unsigned int right_value
    ){
        return (static_cast<std::size_t>(left_byte * 8 + right_byte)
                * pair_stride)
             + static_cast<std::size_t>(left_value) * byte_values
             + right_value;
    }

public:
    ByteProductTable(): products_(byte_pairs * pair_stride){
        std::array<std::array<std::uint64_t, 64>, 64> basis{};
        for(unsigned int left_bit = 0; left_bit < 64; ++left_bit){
            for(unsigned int right_bit = 0; right_bit < 64; ++right_bit){
                basis[left_bit][right_bit] = recursive_product(
                    std::uint64_t{1} << left_bit,
                    std::uint64_t{1} << right_bit,
                    64
                );
            }
        }

        std::array<std::array<std::uint64_t, 8>, byte_values>
            left_products{};
        for(unsigned int left_byte = 0; left_byte < 8; ++left_byte){
            for(unsigned int right_byte = 0; right_byte < 8; ++right_byte){
                for(unsigned int right_bit = 0; right_bit < 8; ++right_bit){
                    left_products[0][right_bit] = 0;
                    for(unsigned int left_value = 1;
                        left_value < byte_values; ++left_value){
                        const unsigned int bit =
                            std::countr_zero(left_value);
                        left_products[left_value][right_bit] =
                            left_products[
                                left_value ^ (1U << bit)
                            ][right_bit]
                            ^ basis[left_byte * 8 + bit]
                                   [right_byte * 8 + right_bit];
                    }
                }
                for(unsigned int left_value = 0;
                    left_value < byte_values; ++left_value){
                    products_[index(
                        left_byte, right_byte, left_value, 0
                    )] = 0;
                    for(unsigned int right_value = 1;
                        right_value < byte_values; ++right_value){
                        const unsigned int bit =
                            std::countr_zero(right_value);
                        products_[index(
                            left_byte, right_byte,
                            left_value, right_value
                        )] = products_[index(
                            left_byte, right_byte, left_value,
                            right_value ^ (1U << bit)
                        )] ^ left_products[left_value][bit];
                    }
                }
            }
        }
    }

    [[nodiscard]] std::uint64_t product(
        std::uint64_t left,
        std::uint64_t right
    ) const{
        std::uint64_t result = 0;
        for(unsigned int left_byte = 0; left_byte < 8; ++left_byte){
            const unsigned int left_value = static_cast<unsigned int>(
                (left >> (left_byte * 8)) & 255U
            );
            if(left_value == 0) continue;
            for(unsigned int right_byte = 0; right_byte < 8; ++right_byte){
                const unsigned int right_value = static_cast<unsigned int>(
                    (right >> (right_byte * 8)) & 255U
                );
                if(right_value == 0) continue;
                result ^= products_[index(
                    left_byte, right_byte, left_value, right_value
                )];
            }
        }
        return result;
    }
};

} // namespace nim_product_internal

inline std::uint64_t nim_product_64(
    std::uint64_t left,
    std::uint64_t right
){
    static const nim_product_internal::ByteProductTable table;
    return table.product(left, right);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_NIM_PRODUCT_64_HPP_INCLUDED
