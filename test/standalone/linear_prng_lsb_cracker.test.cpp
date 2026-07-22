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

#include "../../src/algorithm/random/crack/gf2_linear_state_cracker.hpp"
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

void check_library_contracts(){
    assert(xoroshiro128plus_lsb_observation_count == 128);
    assert(well512a_lsb_observation_count == 512);
    assert(xoroshiro128plus_lsb_observation_rank() == 128);
    assert(well512a_lsb_observation_rank() == 512);

    Gf2LinearStateCracker<4> identity(
        [](std::array<std::uint64_t, 1>& state, std::size_t index){
            const bool bit = ((state[0] >> index) & 1U) != 0;
            return bit;
        }
    );
    const std::array<std::uint8_t, 4> bits{1, 0, 1, 1};
    assert(identity.rank() == 4);
    assert(identity.uniquely_recoverable());
    assert(identity.recover(bits)[0] == 13);

    Gf2LinearStateCracker<4> unobservable(
        [](std::array<std::uint64_t, 1>&, std::size_t){ return false; }
    );
    assert(unobservable.rank() == 0);
    bool logic_error = false;
    try{
        (void)unobservable.recover(bits);
    }catch(const std::logic_error&){
        logic_error = true;
    }
    assert(logic_error);

    const std::array<std::uint8_t, 3> short_bits{};
    std::array<std::uint8_t, 4> invalid_bits{};
    invalid_bits[2] = 2;
    assert(throws_invalid_argument([&]{ (void)identity.recover(short_bits); }));
    assert(throws_invalid_argument([&]{ (void)identity.recover(invalid_bits); }));

    Xoroshiro128PlusState xoroshiro_source{
        0x0123456789abcdefULL,
        0xfedcba9876543210ULL
    };
    const Xoroshiro128PlusState xoroshiro_initial = xoroshiro_source;
    std::array<std::uint8_t, 128> xoroshiro_bits{};
    for(auto& bit: xoroshiro_bits){
        bit = static_cast<std::uint8_t>(
            xoroshiro128plus_next(xoroshiro_source) & 1U
        );
    }
    auto xoroshiro_predictor =
        recover_xoroshiro128plus_state_from_lsb(xoroshiro_bits);
    assert(xoroshiro_predictor == xoroshiro_initial);
    for(auto bit: xoroshiro_bits){
        assert((xoroshiro128plus_next(xoroshiro_predictor) & 1U) == bit);
    }
    for(int index = 0; index < 64; ++index){
        assert(xoroshiro128plus_next(xoroshiro_predictor)
               == xoroshiro128plus_next(xoroshiro_source));
    }

    Well512aState well_source;
    for(std::size_t index = 0; index < well_source.words.size(); ++index){
        well_source.words[index] = static_cast<std::uint32_t>(
            0x9e3779b9U * static_cast<std::uint32_t>(index + 1)
        );
    }
    const Well512aState well_initial = well_source;
    std::array<std::uint8_t, 512> well_bits{};
    for(auto& bit: well_bits){
        bit = static_cast<std::uint8_t>(well512a_next(well_source) & 1U);
    }
    auto well_predictor = recover_well512a_state_from_lsb(well_bits);
    assert(well_predictor == well_initial);
    for(auto bit: well_bits){
        assert((well512a_next(well_predictor) & 1U) == bit);
    }
    for(int index = 0; index < 64; ++index){
        assert(well512a_next(well_predictor) == well512a_next(well_source));
    }

    for(const std::size_t initial_index: {std::size_t{1}, std::size_t{7},
                                          std::size_t{15}}){
        Well512aState shifted_source = well_initial;
        shifted_source.index = initial_index;
        const Well512aState shifted_initial = shifted_source;
        std::array<std::uint8_t, 512> shifted_bits{};
        for(auto& bit: shifted_bits){
            bit = static_cast<std::uint8_t>(
                well512a_next(shifted_source) & 1U
            );
        }
        auto shifted_predictor = recover_well512a_state_from_lsb(
            shifted_bits, initial_index
        );
        assert(shifted_predictor == shifted_initial);
        for(auto bit: shifted_bits){
            assert((well512a_next(shifted_predictor) & 1U) == bit);
        }
        for(int index = 0; index < 64; ++index){
            assert(well512a_next(shifted_predictor)
                   == well512a_next(shifted_source));
        }
    }
    assert(throws_invalid_argument([&]{
        (void)recover_well512a_state_from_lsb(well_bits, 16);
    }));
}

std::string solve_xoroshiro(const std::vector<std::uint8_t>& observations){
    try{
        const auto state = recover_xoroshiro128plus_state_from_lsb(
            observations
        );
        return std::to_string(state.first) + " "
            + std::to_string(state.second);
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

std::string solve_well(const std::vector<std::uint8_t>& observations){
    try{
        const auto state = recover_well512a_state_from_lsb(observations);
        std::string result;
        for(std::size_t index = 0; index < state.words.size(); ++index){
            if(index != 0) result += ' ';
            result += std::to_string(state.words[index]);
        }
        result += " " + std::to_string(state.index);
        return result;
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

}

int main(){
    check_library_contracts();

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
        }else if(engine == "xoroshiro128plus"){
            std::cout << solve_xoroshiro(observations) << '\n';
        }else if(engine == "well512a"){
            std::cout << solve_well(observations) << '\n';
        }else{
            std::cout << "invalid\n";
        }
    }
}
