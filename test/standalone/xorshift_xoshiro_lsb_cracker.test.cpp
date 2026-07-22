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

#include "../../src/algorithm/random/crack/linear_prng_lsb_cracker.hpp"

namespace{

template<class Function>
bool throws_invalid_argument(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

void check_contracts(){
    assert(xorshift128plus_lsb_observation_count == 128);
    assert(xoshiro256plus_lsb_observation_count == 256);
    assert(xorshift128plus_lsb_observation_rank() == 128);
    assert(xoshiro256plus_lsb_observation_rank() == 256);

    Xorshift128PlusState xorshift_source{
        0x0123456789abcdefULL,
        0xfedcba9876543210ULL
    };
    const Xorshift128PlusState xorshift_initial = xorshift_source;
    std::array<std::uint8_t, 128> xorshift_bits{};
    for(auto& bit: xorshift_bits){
        bit = static_cast<std::uint8_t>(
            xorshift128plus_next(xorshift_source) & 1U
        );
    }
    auto xorshift_predictor =
        recover_xorshift128plus_state_from_lsb(xorshift_bits);
    assert(xorshift_predictor == xorshift_initial);
    for(auto bit: xorshift_bits){
        assert((xorshift128plus_next(xorshift_predictor) & 1U) == bit);
    }
    for(int index = 0; index < 64; ++index){
        assert(xorshift128plus_next(xorshift_predictor)
               == xorshift128plus_next(xorshift_source));
    }

    Xoshiro256PlusState xoshiro_source{{
        0x0123456789abcdefULL,
        0xfedcba9876543210ULL,
        0x3141592653589793ULL,
        0x2718281828459045ULL
    }};
    const Xoshiro256PlusState xoshiro_initial = xoshiro_source;
    std::array<std::uint8_t, 256> xoshiro_bits{};
    for(auto& bit: xoshiro_bits){
        bit = static_cast<std::uint8_t>(
            xoshiro256plus_next(xoshiro_source) & 1U
        );
    }
    auto xoshiro_predictor =
        recover_xoshiro256plus_state_from_lsb(xoshiro_bits);
    assert(xoshiro_predictor == xoshiro_initial);
    for(auto bit: xoshiro_bits){
        assert((xoshiro256plus_next(xoshiro_predictor) & 1U) == bit);
    }
    for(int index = 0; index < 64; ++index){
        assert(xoshiro256plus_next(xoshiro_predictor)
               == xoshiro256plus_next(xoshiro_source));
    }

    const std::array<std::uint8_t, 127> short_xorshift{};
    const std::array<std::uint8_t, 255> short_xoshiro{};
    std::array<std::uint8_t, 128> invalid_xorshift{};
    std::array<std::uint8_t, 256> invalid_xoshiro{};
    invalid_xorshift[17] = 2;
    invalid_xoshiro[91] = 255;
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift128plus_state_from_lsb(short_xorshift);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xoshiro256plus_state_from_lsb(short_xoshiro);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xorshift128plus_state_from_lsb(invalid_xorshift);
    }));
    assert(throws_invalid_argument([&]{
        (void)recover_xoshiro256plus_state_from_lsb(invalid_xoshiro);
    }));
}

std::string solve_xorshift(std::span<const std::uint8_t> observations){
    try{
        const auto state =
            recover_xorshift128plus_state_from_lsb(observations);
        return std::to_string(state.first) + " "
            + std::to_string(state.second);
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

std::string solve_xoshiro(std::span<const std::uint8_t> observations){
    try{
        const auto state =
            recover_xoshiro256plus_state_from_lsb(observations);
        std::string result;
        for(std::size_t index = 0; index < state.words.size(); ++index){
            if(index != 0) result += ' ';
            result += std::to_string(state.words[index]);
        }
        return result;
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

}

int main(){
    check_contracts();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string engine;
        int length;
        std::cin >> engine >> length;
        if(length < 0){
            std::cout << "invalid\n";
            continue;
        }
        std::vector<std::uint8_t> observations;
        observations.reserve(static_cast<std::size_t>(length));
        bool invalid_byte = false;
        for(int index = 0; index < length; ++index){
            int value;
            std::cin >> value;
            if(value < 0 || value > 255){
                invalid_byte = true;
                value = 0;
            }
            observations.push_back(static_cast<std::uint8_t>(value));
        }
        if(invalid_byte){
            std::cout << "invalid\n";
        }else if(engine == "xorshift128plus"){
            std::cout << solve_xorshift(observations) << '\n';
        }else if(engine == "xoshiro256plus"){
            std::cout << solve_xoshiro(observations) << '\n';
        }else{
            std::cout << "invalid\n";
        }
    }
}
