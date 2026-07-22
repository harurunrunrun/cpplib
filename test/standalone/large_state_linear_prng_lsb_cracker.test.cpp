// competitive-verifier: STANDALONE

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/random/crack/large_state_linear_prng_lsb_cracker.hpp"

namespace{

[[noreturn]] void fail(const std::string& message){
    throw std::runtime_error(
        "large_state_linear_prng_lsb_cracker.test.cpp: " + message
    );
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

void check_xoshiro512(){
    if(xoshiro512plus_lsb_observation_count != 512
        || xoshiro512plus_lsb_observation_rank() != 512){
        fail("Xoshiro512+ observation matrix is not full rank");
    }
    Xoshiro512PlusState source{{
        0x0123456789abcdefULL, 0xfedcba9876543210ULL,
        0x3141592653589793ULL, 0x2718281828459045ULL,
        0x9e3779b97f4a7c15ULL, 0xbf58476d1ce4e5b9ULL,
        0x94d049bb133111ebULL, 0xda942042e4dd58b5ULL,
    }};
    const Xoshiro512PlusState initial = source;
    std::array<std::uint8_t, 512> bits{};
    for(std::uint8_t& bit: bits){
        bit = static_cast<std::uint8_t>(xoshiro512plus_next(source) & 1U);
    }
    Xoshiro512PlusState recovered =
        recover_xoshiro512plus_state_from_lsb(bits);
    if(recovered != initial) fail("Xoshiro512+ state recovery failed");
    for(const std::uint8_t bit: bits){
        if((xoshiro512plus_next(recovered) & 1U) != bit){
            fail("Xoshiro512+ recovered observations differ");
        }
    }
    for(int step = 0; step < 64; ++step){
        if(xoshiro512plus_next(recovered) != xoshiro512plus_next(source)){
            fail("Xoshiro512+ future output differs");
        }
    }

    const std::array<std::uint8_t, 511> short_bits{};
    std::array<std::uint8_t, 512> invalid_bits{};
    invalid_bits[317] = 2;
    if(!throws_invalid_argument([&]{
        (void)recover_xoshiro512plus_state_from_lsb(short_bits);
    }) || !throws_invalid_argument([&]{
        (void)recover_xoshiro512plus_state_from_lsb(invalid_bits);
    })){
        fail("Xoshiro512+ invalid observations were accepted");
    }
}

void check_xoroshiro1024(){
    if(xoroshiro1024star_lsb_observation_count != 1024
        || xoroshiro1024star_lsb_observation_rank() != 1024){
        fail("Xoroshiro1024* observation matrix is not full rank");
    }
    Xoroshiro1024StarState base;
    for(std::size_t index = 0; index < base.words.size(); ++index){
        base.words[index] =
            0x9e3779b97f4a7c15ULL * static_cast<std::uint64_t>(index + 1);
    }
    for(const std::size_t initial_index:
        {std::size_t{0}, std::size_t{1}, std::size_t{7}, std::size_t{15}}){
        Xoroshiro1024StarState source = base;
        source.index = initial_index;
        const Xoroshiro1024StarState initial = source;
        std::array<std::uint8_t, 1024> bits{};
        for(std::uint8_t& bit: bits){
            bit = static_cast<std::uint8_t>(
                xoroshiro1024star_next(source) & 1U
            );
        }
        Xoroshiro1024StarState recovered =
            recover_xoroshiro1024star_state_from_lsb(bits, initial_index);
        if(recovered != initial){
            fail("Xoroshiro1024* state recovery failed");
        }
        for(const std::uint8_t bit: bits){
            if((xoroshiro1024star_next(recovered) & 1U) != bit){
                fail("Xoroshiro1024* recovered observations differ");
            }
        }
        for(int step = 0; step < 64; ++step){
            if(xoroshiro1024star_next(recovered)
                != xoroshiro1024star_next(source)){
                fail("Xoroshiro1024* future output differs");
            }
        }
    }

    const std::array<std::uint8_t, 1023> short_bits{};
    std::array<std::uint8_t, 1024> invalid_bits{};
    invalid_bits[777] = 255;
    if(!throws_invalid_argument([&]{
        (void)recover_xoroshiro1024star_state_from_lsb(short_bits);
    }) || !throws_invalid_argument([&]{
        (void)recover_xoroshiro1024star_state_from_lsb(invalid_bits);
    }) || !throws_invalid_argument([&]{
        (void)recover_xoroshiro1024star_state_from_lsb(invalid_bits, 16);
    })){
        fail("Xoroshiro1024* invalid observations or index were accepted");
    }
}

template<std::size_t Size>
std::string format_words(const std::array<std::uint64_t, Size>& words){
    std::string result;
    for(std::size_t index = 0; index < Size; ++index){
        if(index != 0) result += ' ';
        result += std::to_string(words[index]);
    }
    return result;
}

std::string solve_xoshiro(std::span<const std::uint8_t> observations){
    try{
        return format_words(
            recover_xoshiro512plus_state_from_lsb(observations).words
        );
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

std::string solve_xoroshiro(
    std::span<const std::uint8_t> observations,
    std::size_t initial_index
){
    try{
        const auto state = recover_xoroshiro1024star_state_from_lsb(
            observations, initial_index
        );
        return format_words(state.words) + " " + std::to_string(state.index);
    }catch(const std::invalid_argument&){
        return "invalid";
    }
}

}  // namespace

int main(){
    check_xoshiro512();
    check_xoroshiro1024();

    int test_count = 0;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string engine;
        long long initial_index = 0;
        int length = 0;
        std::cin >> engine >> initial_index >> length;
        if(length < 0){
            std::cout << "invalid\n";
            continue;
        }
        std::vector<std::uint8_t> observations;
        observations.reserve(static_cast<std::size_t>(length));
        bool invalid = initial_index < 0;
        for(int index = 0; index < length; ++index){
            int value = 0;
            std::cin >> value;
            if(value < 0 || 255 < value){
                invalid = true;
                value = 0;
            }
            observations.push_back(static_cast<std::uint8_t>(value));
        }
        if(invalid){
            std::cout << "invalid\n";
        }else if(engine == "xoshiro512plus" && initial_index == 0){
            std::cout << solve_xoshiro(observations) << '\n';
        }else if(engine == "xoroshiro1024star"){
            std::cout << solve_xoroshiro(
                observations, static_cast<std::size_t>(initial_index)
            ) << '\n';
        }else{
            std::cout << "invalid\n";
        }
    }
}
