#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>

#include "xorshift.hpp"

namespace xorshift_cracker_internal{

template<class UInt>
constexpr UInt undo_left_xor(UInt value, int shift) noexcept{
    static_assert(std::is_unsigned_v<UInt>);
    UInt result = value;
    for(int covered = shift;
        covered < std::numeric_limits<UInt>::digits;
        covered += shift){
        result = value ^ (result << shift);
    }
    return result;
}

template<class UInt>
constexpr UInt undo_right_xor(UInt value, int shift) noexcept{
    static_assert(std::is_unsigned_v<UInt>);
    UInt result = value;
    for(int covered = shift;
        covered < std::numeric_limits<UInt>::digits;
        covered += shift){
        result = value ^ (result >> shift);
    }
    return result;
}

struct XorShift64LsbLinearSystem{
    std::array<std::uint64_t, 64> inverse_rows{};
    std::size_t rank = 0;
};

constexpr std::array<std::uint64_t, 64>
make_xorshift64_lsb_observation_rows(){
    std::array<std::uint64_t, 64> rows{};
    for(std::size_t input_bit = 0; input_bit < 64; ++input_bit){
        XorShift64 engine(std::uint64_t{1} << input_bit);
        for(std::size_t output_index = 0; output_index < 64;
            ++output_index){
            if((engine() & std::uint64_t{1}) != 0){
                rows[output_index] |= std::uint64_t{1} << input_bit;
            }
        }
    }
    return rows;
}

constexpr XorShift64LsbLinearSystem make_xorshift64_lsb_linear_system(){
    auto rows = make_xorshift64_lsb_observation_rows();
    XorShift64LsbLinearSystem system;
    for(std::size_t row = 0; row < 64; ++row){
        system.inverse_rows[row] = std::uint64_t{1} << row;
    }

    for(std::size_t column = 0; column < 64; ++column){
        std::size_t pivot = system.rank;
        while(pivot < 64
            && ((rows[pivot] >> column) & std::uint64_t{1}) == 0){
            ++pivot;
        }
        if(pivot == 64) continue;

        if(pivot != system.rank){
            const std::uint64_t row_temporary = rows[pivot];
            rows[pivot] = rows[system.rank];
            rows[system.rank] = row_temporary;
            const std::uint64_t inverse_temporary =
                system.inverse_rows[pivot];
            system.inverse_rows[pivot] = system.inverse_rows[system.rank];
            system.inverse_rows[system.rank] = inverse_temporary;
        }

        for(std::size_t row = 0; row < 64; ++row){
            if(row != system.rank
                && ((rows[row] >> column) & std::uint64_t{1}) != 0){
                rows[row] ^= rows[system.rank];
                system.inverse_rows[row] ^=
                    system.inverse_rows[system.rank];
            }
        }
        ++system.rank;
    }
    return system;
}

inline constexpr XorShift64LsbLinearSystem xorshift64_lsb_linear_system =
    make_xorshift64_lsb_linear_system();

static_assert(
    xorshift64_lsb_linear_system.rank == 64,
    "the XorShift64 LSB observation matrix must have full rank"
);

}

inline constexpr std::size_t xorshift64_lsb_observation_count = 64;

constexpr std::size_t xorshift64_lsb_observation_rank() noexcept{
    return xorshift_cracker_internal::xorshift64_lsb_linear_system.rank;
}

constexpr std::uint64_t recover_xorshift64_seed_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
){
    if(consecutive_output_lsb.size() != xorshift64_lsb_observation_count){
        throw std::invalid_argument(
            "XorShift64 LSB recovery requires exactly 64 observations"
        );
    }

    std::uint64_t observations = 0;
    for(std::size_t index = 0; index < consecutive_output_lsb.size();
        ++index){
        const std::uint8_t bit = consecutive_output_lsb[index];
        if(bit > 1){
            throw std::invalid_argument(
                "XorShift64 LSB observations must be either 0 or 1"
            );
        }
        observations |= static_cast<std::uint64_t>(bit) << index;
    }

    std::uint64_t seed = 0;
    const auto& inverse_rows =
        xorshift_cracker_internal::xorshift64_lsb_linear_system.inverse_rows;
    for(std::size_t seed_bit = 0; seed_bit < 64; ++seed_bit){
        const unsigned int parity = static_cast<unsigned int>(
            std::popcount(inverse_rows[seed_bit] & observations) & 1
        );
        seed |= static_cast<std::uint64_t>(parity) << seed_bit;
    }
    return seed;
}

constexpr std::uint32_t recover_xorshift32_seed(
    std::uint32_t first_output
) noexcept{
    using namespace xorshift_cracker_internal;
    std::uint32_t value = undo_left_xor(first_output, 5);
    value = undo_right_xor(value, 17);
    return undo_left_xor(value, 13);
}

constexpr std::uint64_t recover_xorshift64_seed(
    std::uint64_t first_output
) noexcept{
    using namespace xorshift_cracker_internal;
    std::uint64_t value = undo_left_xor(first_output, 17);
    value = undo_right_xor(value, 7);
    return undo_left_xor(value, 13);
}

class XorShift128Cracker{
public:
    using result_type = XorShift128::result_type;
    using state_type = XorShift128::state_type;

private:
    state_type initial_state_;
    XorShift128 predictor_;

    static constexpr state_type rewind(state_type state) noexcept{
        using namespace xorshift_cracker_internal;
        for(int step = 0; step < 4; ++step){
            const result_type old_w = state[2];
            result_type temporary = state[3] ^ old_w ^ (old_w >> 19);
            temporary = undo_right_xor(temporary, 8);
            const result_type old_x = undo_left_xor(temporary, 11);
            state = {old_x, state[0], state[1], state[2]};
        }
        return state;
    }

public:
    constexpr explicit XorShift128Cracker(
        const state_type& four_consecutive_outputs
    )
        : initial_state_(rewind(four_consecutive_outputs)),
          predictor_(four_consecutive_outputs){}

    constexpr result_type operator()() noexcept{
        return predictor_();
    }

    constexpr result_type next() noexcept{
        return predictor_();
    }

    constexpr state_type initial_state() const noexcept{
        return initial_state_;
    }

    constexpr state_type current_state() const noexcept{
        return predictor_.state();
    }
};
