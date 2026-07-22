// competitive-verifier: STANDALONE

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "../../src/algorithm/random/engine/philox_engine.hpp"

namespace{

using TinyPhilox = PhiloxEngine<
    std::uint8_t,
    2,
    2,
    1,
    3,
    1
>;

struct FixedSeedSequence{
    template<class Iterator>
    void generate(Iterator first, Iterator last){
        std::uint_least32_t value = 1;
        while(first != last){
            *first++ = value++;
        }
    }
};

bool official_vectors(){
    Philox4x32 engine32;
    engine32.discard(9999);
    if(engine32() != 1955073260ULL) return false;

    Philox4x64 engine64;
    engine64.discard(9999);
    return engine64() == 3409172418970261260ULL;
}

bool seed_and_counter_cases(){
    using result32 = Philox4x32::result_type;
    constexpr result32 seed = static_cast<result32>(0x12345678ULL);
    Philox4x32 engine(seed);
    if(engine.key()[0] != seed || engine.key()[1] != 0) return false;
    if(engine.counter() != Philox4x32::counter_type{}) return false;

    FixedSeedSequence sequence32;
    Philox4x32 from_sequence32(sequence32);
    if(from_sequence32.key()[0] != 1 || from_sequence32.key()[1] != 2){
        return false;
    }

    FixedSeedSequence sequence64;
    Philox4x64 from_sequence64(sequence64);
    using result64 = Philox4x64::result_type;
    if(from_sequence64.key()[0]
            != (result64(1) | (result64(2) << 32))
       || from_sequence64.key()[1]
            != (result64(3) | (result64(4) << 32))){
        return false;
    }

    const result32 maximum = Philox4x32::max();
    Philox4x32 carry(seed);
    carry.set_counter({0, 0, maximum, maximum});
    (void)carry();
    if(carry.counter() != Philox4x32::counter_type{0, 1, 0, 0}){
        return false;
    }

    Philox4x32 wrap(seed);
    wrap.set_counter({maximum, maximum, maximum, maximum});
    (void)wrap();
    if(wrap.counter() != Philox4x32::counter_type{}) return false;

    constexpr std::array<unsigned long long, 8> counts{
        0, 1, 3, 4, 5, 63, 1024, 100003,
    };
    for(const auto count: counts){
        Philox4x32 stepped(seed);
        Philox4x32 jumped(seed);
        for(unsigned long long index = 0; index < count; ++index){
            (void)stepped();
        }
        jumped.discard(count);
        if(!(stepped == jumped) || stepped() != jumped()) return false;
    }
    return true;
}

bool subword_and_two_word_cases(){
    if(TinyPhilox::min() != 0 || TinyPhilox::max() != 3) return false;
    TinyPhilox engine(5);
    if(engine.key()[0] != 1) return false;
    constexpr std::array<unsigned int, 4> expected{1, 0, 1, 3};
    for(const unsigned int value: expected){
        if(static_cast<unsigned int>(engine()) != value) return false;
    }

    engine.set_counter({7, 6});
    if(engine.counter() != TinyPhilox::counter_type{3, 2}) return false;
    (void)engine();
    if(engine.counter() != TinyPhilox::counter_type{3, 3}) return false;

    TinyPhilox wrap(0);
    wrap.set_counter({3, 3});
    (void)wrap();
    return wrap.counter() == TinyPhilox::counter_type{};
}

template<class Engine>
bool round_trip(Engine engine){
    (void)engine();
    (void)engine();
    std::ostringstream output;
    output.setf(std::ios_base::hex, std::ios_base::basefield);
    output.setf(std::ios_base::right, std::ios_base::adjustfield);
    output.fill('#');
    const auto output_flags = output.flags();
    const auto output_fill = output.fill();
    output << engine;
    if(output.flags() != output_flags || output.fill() != output_fill){
        return false;
    }

    std::istringstream input(output.str());
    input.setf(std::ios_base::oct, std::ios_base::basefield);
    input.unsetf(std::ios_base::skipws);
    const auto input_flags = input.flags();
    Engine restored(0);
    input >> restored;
    if(!input || input.flags() != input_flags || !(restored == engine)){
        return false;
    }
    for(int index = 0; index < 24; ++index){
        if(restored() != engine()) return false;
    }
    return true;
}

bool invalid_stream_input_preserves_engine(){
    const std::array<std::string, 5> invalid{
        "4 0 0 1",
        "1 4 0 1",
        "1 0 0 2",
        "-1 0 0 1",
        "1 0",
    };
    for(const std::string& text: invalid){
        TinyPhilox engine(2);
        (void)engine();
        TinyPhilox original = engine;
        std::istringstream input(text);
        input.setf(std::ios_base::hex, std::ios_base::basefield);
        const auto flags = input.flags();
        input >> engine;
        if(!input.fail() || input.flags() != flags || !(engine == original)){
            return false;
        }
        for(int index = 0; index < 8; ++index){
            if(engine() != original()) return false;
        }
    }
    return true;
}

bool stream_cases(){
    TinyPhilox tiny(1);
    (void)tiny();
    std::ostringstream exact;
    exact << tiny;
    if(exact.str() != "1 1 0 0") return false;

    std::istringstream restored_input(exact.str());
    TinyPhilox restored(0);
    restored_input >> restored;
    if(!restored_input || restored() != tiny()) return false;

    return round_trip(Philox4x32(123456789ULL))
        && round_trip(Philox4x64(987654321ULL))
        && invalid_stream_input_preserves_engine();
}

}

int main(){
    std::string mode;
    if(!(std::cin >> mode)) return 1;
    bool success = false;
    if(mode == "official") success = official_vectors();
    else if(mode == "state") success = seed_and_counter_cases();
    else if(mode == "subword") success = subword_and_two_word_cases();
    else if(mode == "stream") success = stream_cases();
    if(!success) return 1;
    std::cout << "OK\n";
    return 0;
}
