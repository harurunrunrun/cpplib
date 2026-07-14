// competitive-verifier: STANDALONE

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/random/philox_cracker.hpp"

namespace{

using TinyPhilox = PhiloxEngine<
    std::uint8_t,
    2,
    2,
    1,
    3,
    1
>;

template<std::size_t Count>
std::array<std::uint8_t, Count> observe(
    std::uint8_t seed,
    unsigned long long discard_count
){
    TinyPhilox engine(seed);
    engine.discard(discard_count);
    std::array<std::uint8_t, Count> result{};
    for(auto& value: result) value = engine();
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

template<class Function>
bool throws_logic_error(Function&& function){
    try{
        function();
    }catch(const std::logic_error&){
        return true;
    }
    return false;
}

bool empty_and_edge_cases(){
    constexpr unsigned long long skip = 3;
    const auto outputs = observe<4>(3, skip);

    auto empty = recover_philox_seed_candidates<TinyPhilox>(
        outputs,
        skip,
        3,
        0
    );
    if(empty.status != PhiloxSeedRecoveryStatus::none
       || empty.unique()
       || empty.recovered_seed()){
        return false;
    }

    auto at_first = recover_philox_seed_candidates<TinyPhilox>(
        outputs,
        skip,
        3,
        2
    );
    if(!at_first.unique() || at_first.recovered_seed() != 3) return false;

    auto at_last = recover_philox_seed_in_range<TinyPhilox>(
        outputs,
        skip,
        2,
        4
    );
    if(!at_last.unique() || at_last.recovered_seed() != 3) return false;

    TinyPhilox expected(3);
    expected.discard(skip + outputs.size());
    for(int index = 0; index < 16; ++index){
        if(at_last.next() != expected()) return false;
    }
    return true;
}

bool maximum_and_alias_cases(){
    constexpr auto maximum = std::numeric_limits<std::uint8_t>::max();
    const auto max_outputs = observe<4>(maximum, 1);
    auto maximum_recovery = recover_philox_seed_candidates<TinyPhilox>(
        max_outputs,
        1,
        static_cast<std::uint8_t>(maximum - 1),
        2
    );
    if(!maximum_recovery.unique()
       || maximum_recovery.recovered_seed() != maximum){
        return false;
    }

    const auto zero_outputs = observe<2>(0, 0);
    auto aliases = recover_philox_seed_candidates<TinyPhilox>(
        zero_outputs,
        0,
        0,
        5
    );
    return aliases.status == PhiloxSeedRecoveryStatus::ambiguous
        && !aliases.unique()
        && !aliases.recovered_seed()
        && throws_logic_error([&]{ (void)aliases.next(); });
}

bool error_cases(){
    const auto outputs = observe<2>(0, 0);
    if(!throws_invalid_argument([&]{
        (void)recover_philox_seed_candidates<TinyPhilox>(
            outputs,
            0,
            254,
            3
        );
    })){
        return false;
    }
    if(!throws_invalid_argument([&]{
        (void)recover_philox_seed_in_range<TinyPhilox>(
            outputs,
            0,
            2,
            1
        );
    })){
        return false;
    }

    auto none = recover_philox_seed_candidates<TinyPhilox>(
        outputs,
        0,
        1,
        1
    );
    return none.status == PhiloxSeedRecoveryStatus::none
        && throws_logic_error([&]{ (void)none.next(); });
}

}

int main(){
    std::string mode;
    if(!(std::cin >> mode)) return 1;
    bool success = false;
    if(mode == "edges") success = empty_and_edge_cases();
    else if(mode == "maximum_aliases") success = maximum_and_alias_cases();
    else if(mode == "errors") success = error_cases();
    if(!success) return 1;
    std::cout << "OK\n";
    return 0;
}
