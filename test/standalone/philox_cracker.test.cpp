// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
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
std::array<std::uint8_t, Count> observe_tiny(
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

bool legacy_empty_and_edge_cases(){
    constexpr unsigned long long skip = 3;
    const auto outputs = observe_tiny<4>(3, skip);

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

bool legacy_maximum_and_alias_cases(){
    constexpr auto maximum = std::numeric_limits<std::uint8_t>::max();
    const auto max_outputs = observe_tiny<4>(maximum, 1);
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

    const auto zero_outputs = observe_tiny<2>(0, 0);
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

bool legacy_error_cases(){
    const auto outputs = observe_tiny<2>(0, 0);
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

template<class Engine>
void check_block_inverse(
    typename Engine::result_type seed,
    const typename Engine::counter_type& counter
){
    Engine source(seed);
    source.set_counter(counter);
    const auto key = source.key();
    typename Engine::block_type expected{};
    for(auto& value: expected) value = source();

    assert(philox_generate_block<Engine>(counter, key) == expected);
    assert(philox_invert_block<Engine>(expected, key) == counter);
}

void test_block_inverse(){
    std::mt19937_64 random(2026071608ULL);

    Philox4x32::counter_type zero32{};
    Philox4x32::counter_type maximum32{};
    maximum32.fill(Philox4x32::max());
    check_block_inverse<Philox4x32>(0, zero32);
    check_block_inverse<Philox4x32>(Philox4x32::max(), maximum32);

    Philox4x64::counter_type zero64{};
    Philox4x64::counter_type maximum64{};
    maximum64.fill(Philox4x64::max());
    check_block_inverse<Philox4x64>(0, zero64);
    check_block_inverse<Philox4x64>(Philox4x64::max(), maximum64);

    for(int iteration = 0; iteration < 2000; ++iteration){
        Philox4x32::counter_type counter32{};
        for(auto& value: counter32){
            value = static_cast<Philox4x32::result_type>(
                random() & Philox4x32::max()
            );
        }
        check_block_inverse<Philox4x32>(
            static_cast<Philox4x32::result_type>(random()),
            counter32
        );

        Philox4x64::counter_type counter64{};
        for(auto& value: counter64){
            value = static_cast<Philox4x64::result_type>(random());
        }
        check_block_inverse<Philox4x64>(
            static_cast<Philox4x64::result_type>(random()),
            counter64
        );
    }
}

void test_one_round_key_recovery(){
    for(unsigned first = 0; first < 4; ++first){
        for(unsigned second = 0; second < 4; ++second){
            const TinyPhilox::counter_type counter{
                static_cast<std::uint8_t>(first),
                static_cast<std::uint8_t>(second),
            };
            for(unsigned seed = 0; seed < 256; ++seed){
                TinyPhilox source(static_cast<std::uint8_t>(seed));
                source.set_counter(counter);
                TinyPhilox::block_type output{};
                for(auto& value: output) value = source();

                const auto key =
                    recover_one_round_philox_key<TinyPhilox>(
                        counter,
                        output
                    );
                assert(key.has_value());
                assert(*key == source.key());

                const auto seed_class =
                    recover_one_round_philox_integer_seed<TinyPhilox>(
                        counter,
                        output
                    );
                assert(seed_class.has_value());
                assert(seed_class->contains(
                    static_cast<std::uint8_t>(seed)
                ));
                assert(
                    seed_class->canonical_seed()
                    == static_cast<std::uint8_t>(seed & 3U)
                );
            }
        }
    }

    TinyPhilox::counter_type counter{};
    TinyPhilox::block_type invalid_output{};
    invalid_output[0] = 4;
    assert(throws_invalid_argument([&]{
        (void)recover_one_round_philox_key<TinyPhilox>(
            counter,
            invalid_output
        );
    }));
}

template<class Engine>
void check_known_key_stream(
    typename Engine::result_type seed,
    unsigned long long discard_count
){
    constexpr std::size_t observation_count =
        PhiloxKnownKeyRecovery<Engine>::minimum_observation_count;
    Engine source(seed);
    source.discard(discard_count);
    const auto key = source.key();
    std::array<typename Engine::result_type, observation_count> outputs{};
    for(auto& value: outputs) value = source();

    auto recovery = recover_philox_known_key_stream<Engine>(
        outputs,
        key
    );
    assert(recovery.unique());
    assert(recovery.candidate_count == 1);
    assert(
        recovery.candidates[0].first_index
        == discard_count % Engine::word_count
    );

    auto standard = recover_philox_integer_seeded_stream<Engine>(
        outputs,
        seed
    );
    assert(standard.unique());
    for(int index = 0; index < 128; ++index){
        const auto expected = source();
        assert(recovery.next() == expected);
        assert(standard.next() == expected);
    }
}

void test_known_key_stream_recovery(){
    for(const auto discard: {
        0ULL,
        1ULL,
        3ULL,
        4ULL,
        5ULL,
        1000ULL,
        std::numeric_limits<unsigned long long>::max(),
    }){
        check_known_key_stream<Philox4x32>(
            static_cast<Philox4x32::result_type>(discard),
            discard
        );
        check_known_key_stream<Philox4x64>(
            static_cast<Philox4x64::result_type>(discard),
            discard
        );
    }

    std::mt19937_64 random(2026071609ULL);
    for(int iteration = 0; iteration < 2000; ++iteration){
        check_known_key_stream<Philox4x32>(
            static_cast<Philox4x32::result_type>(random()),
            random()
        );
        check_known_key_stream<Philox4x64>(
            static_cast<Philox4x64::result_type>(random()),
            random()
        );
    }

    Philox4x32::key_type key{};
    std::array<
        Philox4x32::result_type,
        PhiloxKnownKeyRecovery<Philox4x32>::minimum_observation_count
    > invalid_outputs{};
    invalid_outputs[0] =
        static_cast<Philox4x32::result_type>(1ULL << 32);
    assert(throws_invalid_argument([&]{
        (void)recover_philox_known_key_stream<Philox4x32>(
            invalid_outputs,
            key
        );
    }));
}

template<class Engine>
void print_known_key_recovery(
    typename Engine::result_type seed,
    unsigned long long discard_count,
    std::size_t prediction_count
){
    constexpr std::size_t observation_count =
        PhiloxKnownKeyRecovery<Engine>::minimum_observation_count;
    Engine source(seed);
    source.discard(discard_count);
    std::array<typename Engine::result_type, observation_count> outputs{};
    for(auto& value: outputs) value = source();

    auto recovery = recover_philox_integer_seeded_stream<Engine>(
        outputs,
        seed
    );
    std::cout << recovery.candidate_count;
    for(std::size_t index = 0; index < prediction_count; ++index){
        std::cout << ' ' << recovery.next();
    }
    std::cout << '\n';
}

void print_one_round_recovery(
    std::uint8_t seed,
    std::uint8_t first_counter_word,
    std::uint8_t second_counter_word
){
    TinyPhilox source(seed);
    const TinyPhilox::counter_type counter{
        first_counter_word,
        second_counter_word
    };
    source.set_counter(counter);
    TinyPhilox::block_type output{};
    for(auto& value: output) value = source();

    const auto recovery =
        recover_one_round_philox_integer_seed<TinyPhilox>(
            counter,
            output
        );
    assert(recovery.has_value());
    std::cout << recovery.has_value()
              << ' ' << recovery->contains(seed)
              << ' '
              << static_cast<unsigned>(recovery->canonical_seed())
              << '\n';
}

void self_test(){
    assert(legacy_empty_and_edge_cases());
    assert(legacy_maximum_and_alias_cases());
    assert(legacy_error_cases());
    test_block_inverse();
    test_one_round_key_recovery();
    test_known_key_stream_recovery();
}

} // namespace

int main(){
    self_test();

    std::string mode;
    if(!(std::cin >> mode)) return 0;
    if(mode == "one_round_batch"){
        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            unsigned seed;
            unsigned first_counter_word;
            unsigned second_counter_word;
            std::cin >> seed
                     >> first_counter_word
                     >> second_counter_word;
            assert(seed <= std::numeric_limits<std::uint8_t>::max());
            assert(first_counter_word < 4);
            assert(second_counter_word < 4);
            print_one_round_recovery(
                static_cast<std::uint8_t>(seed),
                static_cast<std::uint8_t>(first_counter_word),
                static_cast<std::uint8_t>(second_counter_word)
            );
        }
        return 0;
    }
    if(mode == "edges"
       || mode == "maximum_aliases"
       || mode == "errors"){
        std::cout << "OK\n";
        return 0;
    }

    const auto run_query = [](const std::string& query_mode){
        std::uint64_t seed;
        unsigned long long discard_count;
        std::size_t prediction_count;
        std::cin >> seed >> discard_count >> prediction_count;
        if(query_mode == "known32"){
            print_known_key_recovery<Philox4x32>(
                static_cast<Philox4x32::result_type>(seed),
                discard_count,
                prediction_count
            );
        }else{
            assert(query_mode == "known64");
            print_known_key_recovery<Philox4x64>(
                static_cast<Philox4x64::result_type>(seed),
                discard_count,
                prediction_count
            );
        }
    };

    if(mode == "batch"){
        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            std::string query_mode;
            std::cin >> query_mode;
            run_query(query_mode);
        }
    }else{
        run_query(mode);
    }
}
