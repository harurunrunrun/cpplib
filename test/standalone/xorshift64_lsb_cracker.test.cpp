// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/random/crack/xorshift_cracker.hpp"

constexpr std::array<std::uint8_t, 64> observe_lsb(std::uint64_t seed){
    std::array<std::uint8_t, 64> observations{};
    std::uint64_t state = seed;
    for(std::size_t index = 0; index < observations.size(); ++index){
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        observations[index] = static_cast<std::uint8_t>(state & 1U);
    }
    return observations;
}

static_assert(xorshift64_lsb_observation_count == 64);
static_assert(xorshift64_lsb_observation_rank() == 64);
static_assert(recover_xorshift64_seed_from_lsb(observe_lsb(1)) == 1);
static_assert(
    recover_xorshift64_seed_from_lsb(
        observe_lsb(XorShift64::default_seed)
    ) == XorShift64::default_seed
);

template<class Function>
void expect_invalid_argument(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void check_seed(std::uint64_t seed){
    const auto observations = observe_lsb(seed);
    assert(recover_xorshift64_seed_from_lsb(observations) == seed);

    if(seed != 0){
        XorShift64 engine(seed);
        for(const std::uint8_t bit: observations){
            assert(static_cast<std::uint8_t>(engine() & 1U) == bit);
        }
    }
}

void test_fixed_and_random_seeds(){
    check_seed(0);
    check_seed(1);
    check_seed(2);
    check_seed(std::uint64_t{1} << 63);
    check_seed(XorShift64::default_seed);
    check_seed(UINT64_MAX);
    for(std::size_t bit = 0; bit < 64; ++bit){
        check_seed(std::uint64_t{1} << bit);
    }

    std::uint64_t random_state = 0x9e3779b97f4a7c15ULL;
    for(int iteration = 0; iteration < 20000; ++iteration){
        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;
        check_seed(random_state);
    }
}

void test_observation_after_previous_outputs(){
    std::uint64_t random_state = 0xd1b54a32d192ed03ULL;
    for(int iteration = 0; iteration < 10000; ++iteration){
        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;
        if(random_state == 0) random_state = 1;

        XorShift64 source(random_state);
        const int skipped = iteration % 257;
        for(int step = 0; step < skipped; ++step) (void)source();
        const std::uint64_t state_before_observation = source.state();

        std::array<std::uint8_t, 64> observations{};
        for(std::uint8_t& bit: observations){
            bit = static_cast<std::uint8_t>(source() & 1U);
        }
        const std::uint64_t recovered =
            recover_xorshift64_seed_from_lsb(observations);
        assert(recovered == state_before_observation);

        XorShift64 predictor(recovered);
        for(const std::uint8_t bit: observations){
            assert(static_cast<std::uint8_t>(predictor() & 1U) == bit);
        }
        for(int step = 0; step < 16; ++step){
            assert(predictor() == source());
        }
    }
}

void test_invalid_observations(){
    std::vector<std::uint8_t> too_short(63, 0);
    std::vector<std::uint8_t> too_long(65, 0);
    std::array<std::uint8_t, 64> invalid_bit{};
    invalid_bit[0] = 2;
    expect_invalid_argument([&]{
        (void)recover_xorshift64_seed_from_lsb(too_short);
    });
    expect_invalid_argument([&]{
        (void)recover_xorshift64_seed_from_lsb(too_long);
    });
    expect_invalid_argument([&]{
        (void)recover_xorshift64_seed_from_lsb(invalid_bit);
    });
}

int main(){
    test_fixed_and_random_seeds();
    test_observation_after_previous_outputs();
    test_invalid_observations();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int length;
        std::cin >> length;
        if(length < 0){
            std::cout << "invalid\n";
            continue;
        }
        std::vector<std::uint8_t> observations;
        observations.reserve(static_cast<std::size_t>(length));
        bool outside_byte = false;
        for(int index = 0; index < length; ++index){
            int value;
            std::cin >> value;
            if(value < 0 || value > 255){
                outside_byte = true;
                value = 0;
            }
            observations.push_back(static_cast<std::uint8_t>(value));
        }
        if(outside_byte){
            std::cout << "invalid\n";
            continue;
        }
        try{
            std::cout << recover_xorshift64_seed_from_lsb(observations)
                      << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "invalid\n";
        }
    }
}
