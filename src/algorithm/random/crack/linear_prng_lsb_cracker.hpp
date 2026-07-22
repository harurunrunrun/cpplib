#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

#include "gf2_linear_state_cracker.hpp"

struct Xoroshiro128PlusState{
    std::uint64_t first = 0;
    std::uint64_t second = 0;

    friend constexpr bool operator==(
        const Xoroshiro128PlusState&,
        const Xoroshiro128PlusState&
    ) = default;
};

constexpr std::uint64_t xoroshiro128plus_next(
    Xoroshiro128PlusState& state
) noexcept{
    const std::uint64_t first = state.first;
    std::uint64_t second = state.second;
    const std::uint64_t result = first + second;
    second ^= first;
    state.first = std::rotl(first, 55) ^ second ^ (second << 14);
    state.second = std::rotl(second, 36);
    return result;
}

struct Well512aState{
    std::array<std::uint32_t, 16> words{};
    std::size_t index = 0;

    friend constexpr bool operator==(
        const Well512aState&,
        const Well512aState&
    ) = default;
};

constexpr std::uint32_t well512a_next(Well512aState& state) noexcept{
    const std::size_t index = state.index;
    std::uint32_t first = state.words[index];
    std::uint32_t third = state.words[(index + 13) & 15U];
    const std::uint32_t second = first ^ third ^ (first << 16)
        ^ (third << 15);
    third = state.words[(index + 9) & 15U];
    third ^= third >> 11;
    first = second ^ third;
    state.words[index] = first;
    const std::uint32_t fourth = first ^ ((first << 5) & 0xda442d24U);
    state.index = (index + 15) & 15U;
    first = state.words[state.index];
    state.words[state.index] = first ^ second ^ fourth ^ (first << 2)
        ^ (second << 18) ^ (third << 28);
    return state.words[state.index];
}

namespace linear_prng_lsb_cracker_internal{

using XoroshiroCracker = Gf2LinearStateCracker<128>;
using WellCracker = Gf2LinearStateCracker<512>;

inline const XoroshiroCracker& xoroshiro_cracker(){
    static const XoroshiroCracker cracker(
        [](XoroshiroCracker::state_type& packed, std::size_t){
            Xoroshiro128PlusState state{packed[0], packed[1]};
            const bool result = (xoroshiro128plus_next(state) & 1U) != 0;
            packed[0] = state.first;
            packed[1] = state.second;
            return result;
        }
    );
    return cracker;
}

inline const WellCracker& well_cracker(){
    static const WellCracker cracker(
        [](WellCracker::state_type& packed, std::size_t output_index){
            Well512aState state;
            for(std::size_t index = 0; index < 16; ++index){
                state.words[index] = static_cast<std::uint32_t>(
                    packed[index / 2] >> (32 * (index % 2))
                );
            }
            state.index = (16 - (output_index & 15U)) & 15U;
            const bool result = (well512a_next(state) & 1U) != 0;
            for(std::size_t index = 0; index < 16; ++index){
                const std::size_t word = index / 2;
                const unsigned int shift = 32U * (index % 2);
                const std::uint64_t mask = std::uint64_t{0xffffffffU}
                    << shift;
                packed[word] = (packed[word] & ~mask)
                    | (std::uint64_t{state.words[index]} << shift);
            }
            return result;
        }
    );
    return cracker;
}

}

inline constexpr std::size_t xoroshiro128plus_lsb_observation_count = 128;
inline constexpr std::size_t well512a_lsb_observation_count = 512;

inline std::size_t xoroshiro128plus_lsb_observation_rank(){
    return linear_prng_lsb_cracker_internal::xoroshiro_cracker().rank();
}

inline std::size_t well512a_lsb_observation_rank(){
    return linear_prng_lsb_cracker_internal::well_cracker().rank();
}

inline Xoroshiro128PlusState recover_xoroshiro128plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
){
    const auto packed =
        linear_prng_lsb_cracker_internal::xoroshiro_cracker().recover(
            consecutive_output_lsb
        );
    return {packed[0], packed[1]};
}

inline Well512aState recover_well512a_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb,
    std::size_t initial_index = 0
){
    if(initial_index >= 16){
        throw std::invalid_argument("WELL512a index must be in [0, 16)");
    }
    const auto packed = linear_prng_lsb_cracker_internal::well_cracker().recover(
        consecutive_output_lsb
    );
    Well512aState result;
    result.index = initial_index;
    for(std::size_t relative_index = 0; relative_index < 16;
        ++relative_index){
        result.words[(initial_index + relative_index) & 15U] =
            static_cast<std::uint32_t>(
            packed[relative_index / 2] >> (32 * (relative_index % 2))
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_LINEAR_PRNG_LSB_CRACKER_HPP_INCLUDED
