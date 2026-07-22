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

#include "../../src/algorithm/random/crack/subtract_with_carry_cracker.hpp"

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

template<class Function>
void expect_logic_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::logic_error&){
        thrown = true;
    }
    assert(thrown);
}

template<class UInt>
class IdentitySeedEngine{
public:
    using result_type = UInt;

private:
    result_type value_;

public:
    explicit IdentitySeedEngine(result_type seed = 0): value_(seed){}

    result_type operator()(){
        return value_;
    }

    void discard(unsigned long long) noexcept{}
};

class ConstantZeroEngine{
public:
    using result_type = std::uint64_t;

    explicit ConstantZeroEngine(result_type = 0){}

    result_type operator()() noexcept{
        return 0;
    }

    void discard(unsigned long long) noexcept{}
};

template<
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag,
    class UInt
>
bool infer_previous_carry(
    const std::array<UInt, LongLag + 1>& observations
){
    using wide_type = __uint128_t;
    const wide_type modulus = wide_type(1) << WordSize;
    const wide_type short_value = observations[LongLag - ShortLag];
    const wide_type long_value = observations[0];
    const wide_type observed = observations[LongLag];
    const auto generated = [&](bool carry){
        return (short_value + modulus - long_value
            - static_cast<unsigned>(carry)) & (modulus - 1);
    };
    const bool matches_without = observed == generated(false);
    const bool matches_with = observed == generated(true);
    assert(matches_without != matches_with);
    return matches_with;
}

template<
    std::size_t LongLag,
    class Engine,
    class Cracker,
    std::size_t Count
>
void check_stream(
    typename Engine::result_type seed,
    unsigned long long skip,
    std::size_t prediction_count = 128
){
    static_assert(LongLag + 1 <= Count);
    Engine source(seed);
    source.discard(skip);
    std::array<typename Engine::result_type, Count> observations{};
    for(auto& value: observations) value = source();

    Cracker cracker(observations);
    const auto state = cracker.state();
    for(std::size_t index = 0; index < LongLag; ++index){
        assert(state[index] == observations[Count - LongLag + index]);
    }
    for(std::size_t index = 0; index < prediction_count; ++index){
        assert(cracker.next() == source());
    }
}

template<
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag,
    class Engine,
    class Cracker
>
void check_carry_branch(
    typename Engine::result_type seed,
    unsigned long long skip,
    bool expected_previous_carry
){
    Engine source(seed);
    source.discard(skip);
    std::array<typename Engine::result_type, LongLag + 1> observations{};
    for(auto& value: observations) value = source();

    const bool previous_carry =
        infer_previous_carry<WordSize, ShortLag, LongLag>(observations);
    assert(previous_carry == expected_previous_carry);

    const auto short_value = observations[LongLag - ShortLag];
    const auto long_value = observations[0];
    const bool expected_current_carry =
        short_value < long_value + static_cast<unsigned>(previous_carry);

    Cracker cracker(observations);
    assert(cracker.carry() == expected_current_carry);
    for(int index = 0; index < 128; ++index){
        assert(cracker.next() == source());
    }
}

void test_minimum_extra_and_carry_observations(){
    check_stream<24, std::ranlux24_base, Ranlux24BaseCracker, 25>(
        123456U, 0
    );
    check_stream<12, std::ranlux48_base, Ranlux48BaseCracker, 13>(
        654U, 11
    );
    check_stream<24, std::ranlux24_base, Ranlux24BaseCracker, 40>(
        987654U, 73
    );
    check_stream<12, std::ranlux48_base, Ranlux48BaseCracker, 29>(
        123456789ULL, 41
    );

    check_carry_branch<
        48, 5, 12, std::ranlux48_base, Ranlux48BaseCracker
    >(321U, 0, false);
    check_carry_branch<
        48, 5, 12, std::ranlux48_base, Ranlux48BaseCracker
    >(654U, 11, true);
}

void test_corrupted_observations(){
    std::ranlux24_base source(24680U);
    std::array<std::ranlux24_base::result_type, 40> observations{};
    for(auto& value: observations) value = source();

    auto corrupted_extra = observations;
    corrupted_extra.back() =
        (corrupted_extra.back() + 1) & Ranlux24BaseCracker::max();
    expect_invalid_argument([&]{
        (void)Ranlux24BaseCracker(corrupted_extra);
    });

    std::array<std::ranlux24_base::result_type, 25> minimum{};
    for(std::size_t index = 0; index < minimum.size(); ++index){
        minimum[index] = observations[index];
    }
    minimum.back() = (minimum.back() + 2) & Ranlux24BaseCracker::max();
    expect_invalid_argument([&]{
        (void)Ranlux24BaseCracker(minimum);
    });

    auto invalid_word = observations;
    invalid_word[7] = std::ranlux24_base::result_type(1) << 24;
    expect_invalid_argument([&]{
        (void)Ranlux24BaseCracker(invalid_word);
    });
}

void test_seed_candidate_boundaries(){
    using Engine = IdentitySeedEngine<std::uint64_t>;
    using Status = SubtractWithCarrySeedRecoveryStatus;
    constexpr std::uint64_t maximum =
        std::numeric_limits<std::uint64_t>::max();

    const std::array<std::uint64_t, 1> maximum_observation{maximum};
    auto empty = recover_subtract_with_carry_seed_candidates<Engine>(
        maximum_observation, 0, maximum, 0
    );
    assert(empty.status == Status::none);
    assert(!empty.unique());
    assert(!empty.recovered_seed().has_value());
    expect_logic_error([&]{ (void)empty.next(); });

    auto maximum_result =
        recover_subtract_with_carry_seed_candidates<Engine>(
            maximum_observation, 0, maximum, 1
        );
    assert(maximum_result.unique());
    assert(maximum_result.recovered_seed() == maximum);
    assert(maximum_result.next() == maximum);

    expect_invalid_argument([&]{
        (void)recover_subtract_with_carry_seed_candidates<Engine>(
            maximum_observation, 0, maximum, 2
        );
    });
    expect_invalid_argument([&]{
        (void)recover_subtract_with_carry_seed_candidates<Engine>(
            maximum_observation, 0, maximum - 1, 3
        );
    });

    const std::array<std::uint64_t, 1> first_observation{10};
    auto first = recover_subtract_with_carry_seed_candidates<Engine>(
        first_observation, 0, 10, 3
    );
    assert(first.unique() && first.recovered_seed() == 10);

    const std::array<std::uint64_t, 1> last_observation{12};
    auto last = recover_subtract_with_carry_seed_candidates<Engine>(
        last_observation, 0, 10, 3
    );
    assert(last.unique() && last.recovered_seed() == 12);

    auto range_first = recover_subtract_with_carry_seed_in_range<Engine>(
        first_observation, 0, 10, 13
    );
    assert(range_first.unique() && range_first.recovered_seed() == 10);
    auto range_last = recover_subtract_with_carry_seed_in_range<Engine>(
        last_observation, 0, 10, 13
    );
    assert(range_last.unique() && range_last.recovered_seed() == 12);
    auto range_empty = recover_subtract_with_carry_seed_in_range<Engine>(
        first_observation, 0, 10, 10
    );
    assert(range_empty.status == Status::none);
    expect_invalid_argument([&]{
        (void)recover_subtract_with_carry_seed_in_range<Engine>(
            first_observation, 0, 11, 10
        );
    });

    const std::array<std::uint64_t, 1> zero_observation{0};
    auto ambiguous =
        recover_subtract_with_carry_seed_candidates<ConstantZeroEngine>(
            zero_observation, 0, 0, 2
        );
    assert(ambiguous.status == Status::ambiguous);
    assert(!ambiguous.recovered_seed().has_value());
    expect_logic_error([&]{ (void)ambiguous.next(); });
}

template<class Engine, std::size_t Count, class Recover>
void check_standard_seed_recovery(
    typename Engine::result_type seed,
    Recover&& recover
){
    Engine source(seed);
    std::array<typename Engine::result_type, Count> observations{};
    for(auto& value: observations) value = source();

    auto recovery = recover(observations);
    assert(recovery.predictor.has_value());
    assert(recovery.unique_seed_class());
    assert(recovery.seed_class_count == 1);
    assert(recovery.contains_seed(seed));
    assert(recovery.canonical_seed().has_value());
    assert(
        recovery.seed_classes[0].contains(
            *recovery.canonical_seed()
        )
    );
    for(int index = 0; index < 128; ++index){
        assert(recovery.next() == source());
    }
}

void test_direct_standard_seed_recovery(){
    using Seed24 = std::ranlux24_base::result_type;
    using Seed48 = std::ranlux48_base::result_type;
    constexpr std::uint64_t lcg_modulus = 2147483563ULL;

    const std::array<Seed24, 7> seeds24{
        0,
        1,
        19780503,
        static_cast<Seed24>(lcg_modulus),
        static_cast<Seed24>(lcg_modulus * 2),
        std::numeric_limits<Seed24>::max(),
        static_cast<Seed24>(123456789),
    };
    for(const Seed24 seed: seeds24){
        check_standard_seed_recovery<std::ranlux24_base, 40>(
            seed,
            [](const auto& outputs){
                return recover_ranlux24_base_standard_seed(outputs);
            }
        );
    }

    const std::array<Seed48, 7> seeds48{
        0,
        1,
        19780503,
        static_cast<Seed48>(lcg_modulus),
        static_cast<Seed48>(lcg_modulus * 2),
        std::numeric_limits<Seed48>::max(),
        static_cast<Seed48>(987654321012345678ULL),
    };
    for(const Seed48 seed: seeds48){
        check_standard_seed_recovery<std::ranlux48_base, 29>(
            seed,
            [](const auto& outputs){
                return recover_ranlux48_base_standard_seed(outputs);
            }
        );
    }

    std::mt19937_64 engine(2026071607ULL);
    for(int iteration = 0; iteration < 512; ++iteration){
        const Seed24 seed24 = static_cast<Seed24>(engine());
        check_standard_seed_recovery<std::ranlux24_base, 40>(
            seed24,
            [](const auto& outputs){
                return recover_ranlux24_base_standard_seed(outputs);
            }
        );
        const Seed48 seed48 = static_cast<Seed48>(engine());
        check_standard_seed_recovery<std::ranlux48_base, 29>(
            seed48,
            [](const auto& outputs){
                return recover_ranlux48_base_standard_seed(outputs);
            }
        );
    }
}

template<class Engine, std::size_t ObservationCount, class Recover>
void print_standard_seed_recovery(
    typename Engine::result_type seed,
    std::size_t prediction_count,
    Recover&& recover
){
    Engine source(seed);
    std::array<typename Engine::result_type, ObservationCount> observations{};
    for(auto& value: observations) value = source();
    auto recovery = recover(observations);

    std::cout
        << recovery.seed_class_count << ' '
        << recovery.contains_seed(seed);
    for(std::size_t index = 0; index < prediction_count; ++index){
        std::cout << ' ' << recovery.next();
    }
    std::cout << '\n';
}

template<
    std::size_t LongLag,
    std::size_t ObservationCount,
    class Engine,
    class Cracker
>
void print_predictions(
    typename Engine::result_type seed,
    unsigned long long skip,
    std::size_t prediction_count
){
    static_assert(LongLag + 1 <= ObservationCount);
    Engine source(seed);
    source.discard(skip);
    std::array<typename Engine::result_type, ObservationCount> observations{};
    for(auto& value: observations) value = source();
    Cracker cracker(observations);

    for(std::size_t index = 0; index < prediction_count; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << cracker.next();
    }
    std::cout << '\n';
}

int main(){
    test_minimum_extra_and_carry_observations();
    test_corrupted_observations();
    test_seed_candidate_boundaries();
    test_direct_standard_seed_recovery();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string mode;
        std::uint64_t seed;
        unsigned long long skip;
        std::size_t prediction_count;
        std::cin >> mode >> seed >> skip >> prediction_count;
        if(mode == "ranlux24_min"){
            print_predictions<
                24, 25, std::ranlux24_base, Ranlux24BaseCracker
            >(static_cast<std::ranlux24_base::result_type>(seed),
              skip, prediction_count);
        }else if(mode == "ranlux24_extra"){
            print_predictions<
                24, 40, std::ranlux24_base, Ranlux24BaseCracker
            >(static_cast<std::ranlux24_base::result_type>(seed),
              skip, prediction_count);
        }else if(mode == "ranlux48_min"){
            print_predictions<
                12, 13, std::ranlux48_base, Ranlux48BaseCracker
            >(static_cast<std::ranlux48_base::result_type>(seed),
              skip, prediction_count);
        }else if(mode == "ranlux24_seed"){
            assert(skip == 0);
            print_standard_seed_recovery<std::ranlux24_base, 40>(
                static_cast<std::ranlux24_base::result_type>(seed),
                prediction_count,
                [](const auto& outputs){
                    return recover_ranlux24_base_standard_seed(outputs);
                }
            );
        }else if(mode == "ranlux48_seed"){
            assert(skip == 0);
            print_standard_seed_recovery<std::ranlux48_base, 29>(
                static_cast<std::ranlux48_base::result_type>(seed),
                prediction_count,
                [](const auto& outputs){
                    return recover_ranlux48_base_standard_seed(outputs);
                }
            );
        }else{
            assert(mode == "ranlux48_extra");
            print_predictions<
                12, 29, std::ranlux48_base, Ranlux48BaseCracker
            >(static_cast<std::ranlux48_base::result_type>(seed),
              skip, prediction_count);
        }
    }
}
