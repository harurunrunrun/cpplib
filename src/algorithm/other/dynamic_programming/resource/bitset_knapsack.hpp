#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_RESOURCE_BITSET_KNAPSACK_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_RESOURCE_BITSET_KNAPSACK_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

struct BitsetKnapsack{
    int capacity;
    std::vector<std::uint64_t> bits;

    explicit BitsetKnapsack(int capacity_)
        : capacity(capacity_ < 0
            ? throw std::runtime_error(
                "library assertion fault: negative capacity "
                "(BitsetKnapsack)."
            )
            : capacity_),
          bits(static_cast<std::size_t>(capacity / 64 + 1), 0){
        bits[0] = 1;
    }

    void add(int weight){
        if(weight < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative weight "
                "(BitsetKnapsack::add)."
            );
        }
        if(weight == 0 || weight > capacity) return;
        const int word_shift = weight / 64;
        const int bit_shift = weight % 64;
        for(int destination = static_cast<int>(bits.size()) - 1;
            destination >= word_shift;
            --destination){
            std::uint64_t shifted =
                bits[static_cast<std::size_t>(
                    destination - word_shift
                )] << bit_shift;
            if(bit_shift != 0 && destination > word_shift){
                shifted |= bits[static_cast<std::size_t>(
                    destination - word_shift - 1
                )] >> (64 - bit_shift);
            }
            bits[static_cast<std::size_t>(destination)] |= shifted;
        }
        const int valid_bits = capacity % 64 + 1;
        if(valid_bits != 64){
            bits.back() &= (std::uint64_t{1} << valid_bits) - 1;
        }
    }

    bool reachable(int weight) const{
        if(weight < 0 || capacity < weight) return false;
        return (
            bits[static_cast<std::size_t>(weight / 64)]
            >> (weight % 64) & 1U
        ) != 0;
    }

    int maximum_reachable() const{
        for(int word = static_cast<int>(bits.size()) - 1;
            word >= 0;
            --word){
            const std::uint64_t value =
                bits[static_cast<std::size_t>(word)];
            if(value == 0) continue;
            return word * 64
                + 63 - static_cast<int>(std::countl_zero(value));
        }
        return 0;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_RESOURCE_BITSET_KNAPSACK_HPP_INCLUDED
