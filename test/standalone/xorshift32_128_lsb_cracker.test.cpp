// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/random/crack/xorshift_cracker.hpp"

namespace{

std::uint32_t next32(std::uint32_t& state){
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

std::uint32_t next128(XorShift128::state_type& state){
    const std::uint32_t temporary = state[0] ^ (state[0] << 11);
    state[0] = state[1];
    state[1] = state[2];
    state[2] = state[3];
    state[3] = state[3] ^ (state[3] >> 19)
        ^ temporary ^ (temporary >> 8);
    return state[3];
}

std::array<std::uint8_t, 32> observe32(std::uint32_t state){
    std::array<std::uint8_t, 32> result{};
    for(auto& bit: result){
        bit = static_cast<std::uint8_t>(next32(state) & 1U);
    }
    return result;
}

std::array<std::uint8_t, 128> observe128(
    XorShift128::state_type state
){
    std::array<std::uint8_t, 128> result{};
    for(auto& bit: result){
        bit = static_cast<std::uint8_t>(next128(state) & 1U);
    }
    return result;
}

template<class Function>
bool throws_invalid_argument(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

void check32(std::uint32_t state){
    const auto observations = observe32(state);
    const std::uint32_t recovered =
        recover_xorshift32_seed_from_lsb(observations);
    assert(recovered == state);

    std::uint32_t expected = state;
    std::uint32_t predicted = recovered;
    for(int index = 0; index < 96; ++index){
        assert(next32(predicted) == next32(expected));
    }
}

void check128(const XorShift128::state_type& state){
    const auto observations = observe128(state);
    const auto recovered =
        recover_xorshift128_state_from_lsb(observations);
    assert(recovered == state);

    auto expected = state;
    auto predicted = recovered;
    for(int index = 0; index < 192; ++index){
        assert(next128(predicted) == next128(expected));
    }
}

void self_test(){
    assert(xorshift32_lsb_observation_count == 32);
    assert(xorshift128_lsb_observation_count == 128);
    assert(xorshift32_lsb_observation_rank() == 32);
    assert(xorshift128_lsb_observation_rank() == 128);

    check32(0);
    check32(1);
    check32(XorShift32::default_seed);
    check32(UINT32_MAX);
    for(std::size_t bit = 0; bit < 32; ++bit){
        check32(std::uint32_t{1} << bit);
    }

    check128({0, 0, 0, 0});
    check128(XorShift128::default_seed);
    check128({UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX});
    for(std::size_t bit = 0; bit < 128; ++bit){
        XorShift128::state_type state{};
        state[bit / 32] = std::uint32_t{1} << (bit % 32);
        check128(state);
    }

    std::uint64_t random = 0x9e3779b97f4a7c15ULL;
    for(int iteration = 0; iteration < 2048; ++iteration){
        random ^= random << 7;
        random ^= random >> 9;
        random ^= random << 8;
        std::uint32_t state32 = static_cast<std::uint32_t>(random);
        for(int skip = 0; skip < iteration % 97; ++skip) next32(state32);
        check32(state32);

        XorShift128::state_type state128{};
        for(auto& word: state128){
            random ^= random << 7;
            random ^= random >> 9;
            random ^= random << 8;
            word = static_cast<std::uint32_t>(random);
        }
        for(int skip = 0; skip < iteration % 131; ++skip){
            next128(state128);
        }
        check128(state128);
    }

    const std::array<std::uint8_t, 31> short32{};
    const std::array<std::uint8_t, 33> long32{};
    const std::array<std::uint8_t, 127> short128{};
    const std::array<std::uint8_t, 129> long128{};
    std::array<std::uint8_t, 32> invalid32{};
    std::array<std::uint8_t, 128> invalid128{};
    invalid32[7] = 2;
    invalid128[91] = 255;
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift32_seed_from_lsb(short32);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift32_seed_from_lsb(long32);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift128_state_from_lsb(short128);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift128_state_from_lsb(long128);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift32_seed_from_lsb(invalid32);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift128_state_from_lsb(invalid128);
    }));
}

std::string solve32(std::span<const std::uint8_t> observations){
    try{
        return std::to_string(
            recover_xorshift32_seed_from_lsb(observations)
        );
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

std::string solve128(std::span<const std::uint8_t> observations){
    try{
        const auto state =
            recover_xorshift128_state_from_lsb(observations);
        return std::to_string(state[0]) + " "
            + std::to_string(state[1]) + " "
            + std::to_string(state[2]) + " "
            + std::to_string(state[3]);
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

}  // namespace

int main(){
    self_test();

    int test_count = 0;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string engine;
        int length = 0;
        std::cin >> engine >> length;
        if(length < 0){
            std::cout << "invalid\n";
            continue;
        }

        std::vector<std::uint8_t> observations;
        observations.reserve(static_cast<std::size_t>(length));
        bool invalid_byte = false;
        for(int index = 0; index < length; ++index){
            int value = 0;
            std::cin >> value;
            if(value < 0 || 255 < value){
                invalid_byte = true;
                value = 0;
            }
            observations.push_back(static_cast<std::uint8_t>(value));
        }

        if(invalid_byte){
            std::cout << "invalid\n";
        }else if(engine == "xorshift32"){
            std::cout << solve32(observations) << '\n';
        }else if(engine == "xorshift128"){
            std::cout << solve128(observations) << '\n';
        }else{
            std::cout << "invalid\n";
        }
    }
}
