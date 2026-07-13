#pragma once

#include <array>
#include <cstdint>
#include <limits>
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
