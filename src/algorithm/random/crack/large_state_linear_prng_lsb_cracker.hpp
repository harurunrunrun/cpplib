#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LARGE_STATE_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LARGE_STATE_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

#include "gf2_linear_state_cracker.hpp"

struct Xoshiro512PlusState{
    std::array<std::uint64_t, 8> words{};

    friend constexpr bool operator==(
        const Xoshiro512PlusState&,
        const Xoshiro512PlusState&
    ) = default;
};

constexpr std::uint64_t xoshiro512plus_next(
    Xoshiro512PlusState& state
) noexcept{
    const std::uint64_t result = state.words[0] + state.words[2];
    const std::uint64_t shifted = state.words[1] << 11;
    state.words[2] ^= state.words[0];
    state.words[5] ^= state.words[1];
    state.words[1] ^= state.words[2];
    state.words[7] ^= state.words[3];
    state.words[3] ^= state.words[4];
    state.words[4] ^= state.words[5];
    state.words[0] ^= state.words[6];
    state.words[6] ^= state.words[7];
    state.words[6] ^= shifted;
    state.words[7] = std::rotl(state.words[7], 21);
    return result;
}

struct Xoroshiro1024StarState{
    std::array<std::uint64_t, 16> words{};
    std::size_t index = 0;

    friend constexpr bool operator==(
        const Xoroshiro1024StarState&,
        const Xoroshiro1024StarState&
    ) = default;
};

constexpr std::uint64_t xoroshiro1024star_next(
    Xoroshiro1024StarState& state
) noexcept{
    const std::size_t previous = state.index;
    state.index = (state.index + 1) & 15U;
    const std::uint64_t first = state.words[state.index];
    std::uint64_t last = state.words[previous];
    const std::uint64_t result =
        first * UINT64_C(0x9e3779b97f4a7c13);
    last ^= first;
    state.words[previous] = std::rotl(first, 25) ^ last ^ (last << 27);
    state.words[state.index] = std::rotl(last, 36);
    return result;
}

namespace large_state_linear_prng_lsb_cracker_detail{

using Xoshiro512Cracker = Gf2LinearStateCracker<512>;
using Xoroshiro1024Cracker = Gf2LinearStateCracker<1024>;

inline const Xoshiro512Cracker& xoshiro512_cracker(){
    static const Xoshiro512Cracker cracker(
        [](Xoshiro512Cracker::state_type& packed, std::size_t){
            Xoshiro512PlusState state{packed};
            const bool result = (xoshiro512plus_next(state) & 1U) != 0;
            packed = state.words;
            return result;
        }
    );
    return cracker;
}

inline const Xoroshiro1024Cracker& xoroshiro1024_cracker(){
    static const Xoroshiro1024Cracker cracker(
        [](Xoroshiro1024Cracker::state_type& packed,
           std::size_t output_index){
            Xoroshiro1024StarState state{
                packed,
                output_index & 15U,
            };
            const bool result = (xoroshiro1024star_next(state) & 1U) != 0;
            packed = state.words;
            return result;
        }
    );
    return cracker;
}

}  // namespace large_state_linear_prng_lsb_cracker_detail

inline constexpr std::size_t xoshiro512plus_lsb_observation_count = 512;
inline constexpr std::size_t xoroshiro1024star_lsb_observation_count = 1024;

inline std::size_t xoshiro512plus_lsb_observation_rank(){
    return large_state_linear_prng_lsb_cracker_detail::
        xoshiro512_cracker().rank();
}

inline std::size_t xoroshiro1024star_lsb_observation_rank(){
    return large_state_linear_prng_lsb_cracker_detail::
        xoroshiro1024_cracker().rank();
}

inline Xoshiro512PlusState recover_xoshiro512plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
){
    return {
        large_state_linear_prng_lsb_cracker_detail::
            xoshiro512_cracker().recover(consecutive_output_lsb)
    };
}

inline Xoroshiro1024StarState recover_xoroshiro1024star_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb,
    std::size_t initial_index = 0
){
    if(initial_index >= 16){
        throw std::invalid_argument(
            "Xoroshiro1024* index must be in [0, 16)"
        );
    }
    const auto relative = large_state_linear_prng_lsb_cracker_detail::
        xoroshiro1024_cracker().recover(consecutive_output_lsb);
    Xoroshiro1024StarState result;
    result.index = initial_index;
    for(std::size_t offset = 0; offset < 16; ++offset){
        result.words[(initial_index + offset) & 15U] = relative[offset];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LARGE_STATE_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED
