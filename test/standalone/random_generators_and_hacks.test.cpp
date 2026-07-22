// competitive-verifier: STANDALONE

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../src/algorithm/random/crack/linear_congruential_cracker.hpp"
#include "../../src/algorithm/random/engine/linear_congruential_generator.hpp"
#include "../../src/algorithm/random/crack/mersenne_twister_cracker.hpp"
#include "../../src/algorithm/random/crack/philox_cracker.hpp"
#include "../../src/algorithm/random/engine/philox_engine.hpp"
#include "../../src/algorithm/random/crack/subtract_with_carry_cracker.hpp"
#include "../../src/algorithm/random/engine/xorshift.hpp"
#include "../../src/algorithm/random/crack/xorshift_cracker.hpp"

namespace{

using WrapLcg = LinearCongruentialGenerator<
    std::uint64_t,
    6364136223846793005ULL,
    1442695040888963407ULL,
    0ULL
>;

template<class Exception, class Function>
bool throws_as(Function&& function){
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        return true;
    }catch(...){
    }
    return false;
}

struct FixedSeedSequence{
    using result_type = std::uint32_t;

    template<class Iterator>
    void generate(Iterator first, Iterator last){
        result_type value = 1;
        while(first != last){
            *first = value;
            ++first;
            ++value;
        }
    }
};

class CollisionEngine{
public:
    using result_type = std::uint32_t;

private:
    result_type value_ = 0;

public:
    explicit CollisionEngine(result_type seed = 0) noexcept
        : value_(seed & 1U){}

    void discard(unsigned long long) noexcept{}

    result_type operator()() noexcept{
        return value_;
    }
};

static_assert(Philox4x32::round_consts[0] == 0x9E3779B9ULL);
static_assert(Philox4x32::round_consts[1] == 0xBB67AE85ULL);
static_assert(Philox4x64::round_consts[0] == 0x9E3779B97F4A7C15ULL);
static_assert(Philox4x64::round_consts[1] == 0xBB67AE8584CAA73BULL);

bool philox_known_answers(){
    Philox4x32 engine32;
    engine32.discard(9999);
    if(engine32() != 1955073260ULL) return false;

    Philox4x64 engine64;
    engine64.discard(9999);
    return engine64() == 3409172418970261260ULL;
}
template<class LocalEngine, class StandardEngine>
bool check_standard_lcg(){
    constexpr std::array<std::uint32_t, 5> seeds{
        0U,
        1U,
        2147483646U,
        2147483647U,
        std::numeric_limits<std::uint32_t>::max(),
    };
    for(const std::uint32_t seed: seeds){
        LocalEngine local(seed);
        StandardEngine standard(seed);
        for(int index = 0; index < 64; ++index){
            if(local() != standard()) return false;
        }

        local.seed(seed);
        standard.seed(seed);
        local.discard(10007);
        standard.discard(10007);
        for(int index = 0; index < 16; ++index){
            if(local() != standard()) return false;
        }
    }
    return LocalEngine::min() == StandardEngine::min()
        && LocalEngine::max() == StandardEngine::max();
}

template<class Engine>
typename Engine::counter_type philox_counter_from_blocks(
    unsigned long long blocks
){
    typename Engine::counter_type result{};
    if constexpr(Engine::word_size == 32){
        result[Engine::word_count - 1] =
            static_cast<typename Engine::result_type>(
                blocks & 0xFFFFFFFFULL
            );
        result[Engine::word_count - 2] =
            static_cast<typename Engine::result_type>(blocks >> 32);
    }else{
        result[Engine::word_count - 1] =
            static_cast<typename Engine::result_type>(blocks);
    }
    return result;
}

template<class Engine>
bool check_philox_public_api(){
    using result_type = typename Engine::result_type;
    constexpr result_type seed = static_cast<result_type>(0x12345678ULL);

    std::uint32_t scalar_lvalue = 123456789U;
    Engine scalar_engine(scalar_lvalue);
    scalar_engine.seed(scalar_lvalue);
    if(scalar_engine.key()[0] != static_cast<result_type>(scalar_lvalue)){
        return false;
    }

    Engine engine(seed);
    const auto initial_key = engine.key();
    if(initial_key[0] != seed) return false;
    for(std::size_t index = 1; index < initial_key.size(); ++index){
        if(initial_key[index] != 0) return false;
    }
    if(engine.index() != Engine::word_count - 1) return false;
    if(engine.counter() != typename Engine::counter_type{}) return false;
    if(Engine::min() != 0) return false;
    if constexpr(Engine::word_size == 32){
        if(Engine::max() != static_cast<result_type>(0xFFFFFFFFULL)){
            return false;
        }
    }else{
        if(Engine::max() != std::numeric_limits<result_type>::max()){
            return false;
        }
    }

    FixedSeedSequence sequence;
    Engine from_sequence(sequence);
    const auto sequence_key = from_sequence.key();
    if constexpr(Engine::word_size == 32){
        if(sequence_key[0] != 1 || sequence_key[1] != 2) return false;
    }else{
        if(sequence_key[0] !=
                (result_type(1) | (result_type(2) << 32))
            || sequence_key[1] !=
                (result_type(3) | (result_type(4) << 32))){
            return false;
        }
    }
    Engine reseeded(seed);
    FixedSeedSequence second_sequence;
    reseeded.seed(second_sequence);
    if(!(reseeded == from_sequence)) return false;

    typename Engine::counter_type positioned_counter{};
    for(std::size_t index = 0; index < Engine::word_count; ++index){
        positioned_counter[index] = static_cast<result_type>(index + 1);
    }
    engine.set_counter(positioned_counter);
    if(engine.counter() != positioned_counter
       || engine.index() != Engine::word_count - 1){
        return false;
    }
    (void)engine();
    ++positioned_counter.back();
    if(engine.counter() != positioned_counter || engine.index() != 0){
        return false;
    }

    constexpr unsigned long long huge =
        std::numeric_limits<unsigned long long>::max();
    for(const unsigned long long count:
        {huge, (1ULL << 62), 4ULL, 5ULL}){
        const auto blocks = count / Engine::word_count;
        const auto remainder = static_cast<std::size_t>(
            count % Engine::word_count
        );
        Engine jumped(seed);
        Engine reference(seed);
        jumped.discard(count);
        reference.set_counter(philox_counter_from_blocks<Engine>(blocks));
        for(std::size_t index = 0; index < remainder; ++index){
            (void)reference();
        }
        if(!(jumped == reference)
           || jumped.counter() != reference.counter()
           || jumped.index() != reference.index()
           || jumped() != reference()){
            return false;
        }
    }

    Engine jumped_from_buffer(seed);
    Engine stepped_from_buffer(seed);
    (void)jumped_from_buffer();
    (void)stepped_from_buffer();
    jumped_from_buffer.discard(9);
    for(int index = 0; index < 9; ++index) (void)stepped_from_buffer();
    return jumped_from_buffer == stepped_from_buffer
        && jumped_from_buffer() == stepped_from_buffer();
}
template<class Cracker>
bool check_mt_public_api(const Cracker& original){
    Cracker jumped = original;
    Cracker stepped = original;
    jumped.discard(17);
    for(int index = 0; index < 17; ++index) (void)stepped();
    return jumped.state() == stepped.state()
        && jumped() == stepped();
}

template<class Cracker>
bool check_swc_public_api(const Cracker& original){
    Cracker jumped = original;
    Cracker stepped = original;
    jumped.discard(17);
    for(int index = 0; index < 17; ++index) (void)stepped();
    return jumped.state() == stepped.state()
        && jumped.carry() == stepped.carry()
        && jumped() == stepped();
}


bool deterministic_self_checks(){
    if(!throws_as<std::invalid_argument>([]{
            (void)XorShift32(0);
        })
       || !throws_as<std::invalid_argument>([]{
            (void)XorShift64(0);
        })
       || !throws_as<std::invalid_argument>([]{
            (void)XorShift128(XorShift128::state_type{});
        })
       || !throws_as<std::invalid_argument>([]{
            (void)XorShift128Cracker(XorShift128::state_type{});
        })){
        return false;
    }
    {
        XorShift32 engine32;
        XorShift64 engine64;
        XorShift128 engine128;
        if(!throws_as<std::invalid_argument>([&]{ engine32.seed(0); })
           || !throws_as<std::invalid_argument>([&]{ engine64.seed(0); })
           || !throws_as<std::invalid_argument>(
                [&]{ engine128.seed(XorShift128::state_type{}); }
           )){
            return false;
        }
    }

    if(!check_standard_lcg<MinStdRand0, std::minstd_rand0>()
       || !check_standard_lcg<MinStdRand, std::minstd_rand>()){
        return false;
    }
    {
        using SmallWrap = LinearCongruentialGenerator<
            std::uint8_t,
            std::uint8_t(5),
            std::uint8_t(1),
            std::uint8_t(0)
        >;
        constexpr std::array<unsigned long long, 8> counts{
            0, 1, 2, 7, 31, 255, 256, 1000,
        };
        for(const unsigned long long count: counts){
            SmallWrap stepped(250);
            SmallWrap jumped(250);
            for(unsigned long long index = 0; index < count; ++index){
                (void)stepped();
            }
            jumped.discard(count);
            if(stepped.state() != jumped.state()) return false;
        }
        SmallWrap huge_stepped(123);
        SmallWrap huge_jumped(123);
        for(int index = 0; index < 255; ++index) (void)huge_stepped();
        huge_jumped.discard(
            std::numeric_limits<unsigned long long>::max()
        );
        if(huge_stepped.state() != huge_jumped.state()
           || SmallWrap::min() != 0
           || SmallWrap::max() != std::numeric_limits<std::uint8_t>::max()){
            return false;
        }

        using NonCoprime = LinearCongruentialGenerator<
            std::uint8_t,
            std::uint8_t(2),
            std::uint8_t(0),
            std::uint8_t(4)
        >;
        NonCoprime non_coprime(2);
        if(NonCoprime::min() != 0 || NonCoprime::max() != 3
           || non_coprime() != 0){
            return false;
        }
        LinearCongruentialCracker<NonCoprime> noninvertible(0);
        if(noninvertible.recovered_seed()) return false;
        constexpr std::array<std::uint8_t, 3> constant_outputs{0, 0, 0};
        if(recover_linear_congruential_parameters<
                std::uint8_t,
                std::uint8_t(4)
            >(constant_outputs)){
            return false;
        }
    }

    {
        const std::array<std::uint32_t, 1> observed{0};
        auto ambiguous =
            recover_subtract_with_carry_seed_in_range<CollisionEngine>(
                observed,
                0,
                0,
                3
            );
        if(ambiguous.status
                != SubtractWithCarrySeedRecoveryStatus::ambiguous
           || ambiguous.unique()
           || ambiguous.recovered_seed()
           || !throws_as<std::logic_error>([&]{ (void)ambiguous.next(); })){
            return false;
        }

        const std::array<std::uint32_t, 1> impossible{2};
        auto none =
            recover_subtract_with_carry_seed_in_range<CollisionEngine>(
                impossible,
                0,
                0,
                3
            );
        if(none.status != SubtractWithCarrySeedRecoveryStatus::none
           || none.unique()
           || none.recovered_seed()
           || !throws_as<std::logic_error>([&]{ (void)none.next(); })
           || !throws_as<std::invalid_argument>([&]{
                (void)recover_subtract_with_carry_seed_in_range<
                    CollisionEngine
                >(observed, 0, 2, 1);
           })){
            return false;
        }
    }

    if(!check_philox_public_api<Philox4x32>()
       || !check_philox_public_api<Philox4x64>()){
        return false;
    }
    {
        using ToyPhilox = PhiloxEngine<
            std::uint8_t,
            2,
            2,
            1,
            1,
            0
        >;
        const std::array<std::uint8_t, 1> observed{0};
        auto ambiguous = recover_philox_seed_in_range<ToyPhilox>(
            observed,
            1,
            0,
            4
        );
        if(ambiguous.status != PhiloxSeedRecoveryStatus::ambiguous
           || ambiguous.unique()
           || ambiguous.recovered_seed()
           || !throws_as<std::logic_error>([&]{ (void)ambiguous.next(); })){
            return false;
        }

        const std::array<std::uint8_t, 1> impossible{4};
        auto none = recover_philox_seed_in_range<ToyPhilox>(
            impossible,
            0,
            0,
            4
        );
        if(none.status != PhiloxSeedRecoveryStatus::none
           || none.unique()
           || none.recovered_seed()
           || !throws_as<std::logic_error>([&]{ (void)none.next(); })
           || !throws_as<std::invalid_argument>([&]{
                (void)recover_philox_seed_in_range<ToyPhilox>(
                    observed,
                    0,
                    2,
                    1
                );
           })){
            return false;
        }
    }

    for(int bit = 0; bit < 32; ++bit){
        const std::uint32_t seed = std::uint32_t(1) << bit;
        XorShift32 engine(seed);
        if(recover_xorshift32_seed(engine()) != seed) return false;
    }
    for(int bit = 0; bit < 64; ++bit){
        const std::uint64_t seed = std::uint64_t(1) << bit;
        XorShift64 engine(seed);
        if(recover_xorshift64_seed(engine()) != seed) return false;
    }

    {
        MinStdRand0 stepped(1234567);
        MinStdRand0 jumped(1234567);
        for(int count = 0; count < 100000; ++count) (void)stepped();
        jumped.discard(100000);
        if(stepped.state() != jumped.state()) return false;

        const auto first = stepped();
        LinearCongruentialCracker<MinStdRand0> cracker(first);
        const auto recovered = cracker.recovered_seed();
        for(int count = 0; count < 100; ++count) (void)cracker();
        if(cracker.recovered_seed() != recovered) return false;
    }
    {
        WrapLcg stepped(987654321);
        WrapLcg jumped(987654321);
        for(int count = 0; count < 100000; ++count) (void)stepped();
        jumped.discard(100000);
        if(stepped.state() != jumped.state()) return false;
    }

    {
        std::mt19937 engine(17);
        std::array<std::uint32_t, 625> outputs{};
        for(auto& value: outputs){
            value = static_cast<std::uint32_t>(engine());
        }
        outputs.back() ^= 1;
        try{
            (void)Mt19937Cracker(outputs);
            return false;
        }catch(const std::invalid_argument&){
        }
    }

    {
        std::ranlux24_base engine(29);
        std::array<std::uint_fast32_t, 25> outputs{};
        for(auto& value: outputs) value = engine();
        outputs.back() ^= 4;
        try{
            (void)Ranlux24BaseCracker(outputs);
            return false;
        }catch(const std::invalid_argument&){
        }
    }

    {
        constexpr std::array<unsigned long long, 8> counts{
            0, 1, 3, 4, 5, 63, 1024, 100000,
        };
        for(unsigned long long count: counts){
            Philox4x32 stepped(314159);
            Philox4x32 jumped(314159);
            for(unsigned long long index = 0; index < count; ++index){
                (void)stepped();
            }
            jumped.discard(count);
            if(!(stepped == jumped) || stepped() != jumped()) return false;
        }

        Philox4x32 positioned(271828);
        positioned.set_counter({0, 0, 0, 7});
        Philox4x32 reference(271828);
        reference.discard(28);
        if(positioned() != reference()) return false;
    }

    return true;
}

int run_xorshift(){
    std::uint32_t seed32;
    std::uint64_t seed64;
    XorShift128::state_type seed128{};
    int prediction_count;
    if(!(std::cin >> seed32 >> seed64
         >> seed128[0] >> seed128[1] >> seed128[2] >> seed128[3]
         >> prediction_count)){
        return 1;
    }

    XorShift32 engine32(seed32);
    XorShift64 engine64(seed64);
    XorShift128 engine128(seed128);
    if(engine32.state() != seed32 || engine64.state() != seed64
       || engine128.state() != seed128
       || XorShift32::min() != 1
       || XorShift32::max() != std::numeric_limits<std::uint32_t>::max()
       || XorShift64::min() != 1
       || XorShift64::max() != std::numeric_limits<std::uint64_t>::max()
       || XorShift128::min() != 0
       || XorShift128::max() != std::numeric_limits<std::uint32_t>::max()){
        return 1;
    }
    const auto first32 = engine32();
    const auto first64 = engine64();
    std::array<std::uint32_t, 4> observed128{};
    for(auto& value: observed128) value = engine128();

    const auto recovered32 = recover_xorshift32_seed(first32);
    const auto recovered64 = recover_xorshift64_seed(first64);
    XorShift128Cracker cracker128(observed128);
    if(recovered32 != seed32 || recovered64 != seed64
       || cracker128.initial_state() != seed128){
        return 1;
    }

    std::cout << recovered32 << '\n' << recovered64 << '\n';
    const auto recovered128 = cracker128.initial_state();
    std::cout << recovered128[0] << ' ' << recovered128[1] << ' '
              << recovered128[2] << ' ' << recovered128[3] << '\n';

    XorShift32 predictor32(first32);
    XorShift64 predictor64(first64);
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted32 = predictor32();
        const auto predicted64 = predictor64();
        const auto predicted128 = cracker128();
        if(predicted32 != engine32() || predicted64 != engine64()
           || predicted128 != engine128()){
            return 1;
        }
        std::cout << predicted32 << ' ' << predicted64 << ' '
                  << predicted128 << '\n';
    }
    return 0;
}

int run_lcg(){
    std::uint32_t seed32;
    std::uint64_t seed64;
    int prediction_count;
    if(!(std::cin >> seed32 >> seed64 >> prediction_count)) return 1;

    MinStdRand0 engine32(seed32);
    WrapLcg engine64(seed64);
    const auto first32 = engine32();
    const auto first64 = engine64();
    LinearCongruentialCracker<MinStdRand0> cracker32(first32);
    LinearCongruentialCracker<WrapLcg> cracker64(first64);
    const auto recovered32 = cracker32.recovered_seed();
    const auto recovered64 = cracker64.recovered_seed();
    const auto normalized32 = seed32 % MinStdRand0::modulus;
    const auto expected32 =
        normalized32 == 0 ? MinStdRand0::result_type(1) : normalized32;
    if(!recovered32 || !recovered64) return 1;
    if(*recovered32 != expected32
       || *recovered64 != seed64){
        return 1;
    }

    std::array<std::uint32_t, 6> observations{};
    MinStdRand0 parameter_engine(seed32);
    for(auto& value: observations) value = parameter_engine();
    const auto parameters =
        recover_linear_congruential_parameters<
            std::uint32_t,
            MinStdRand0::modulus
        >(observations);
    if(!parameters
       || parameters->multiplier != MinStdRand0::multiplier
       || parameters->increment != MinStdRand0::increment){
        return 1;
    }

    std::cout << *recovered32 << '\n' << *recovered64 << '\n';
    std::cout << parameters->multiplier << ' '
              << parameters->increment << '\n';
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted32 = cracker32();
        const auto predicted64 = cracker64();
        if(predicted32 != engine32() || predicted64 != engine64()) return 1;
        std::cout << predicted32 << ' ' << predicted64 << '\n';
    }
    return 0;
}

int run_mt32(){
    std::uint32_t seed;
    unsigned long long skip;
    int prediction_count;
    if(!(std::cin >> seed >> skip >> prediction_count)) return 1;
    std::mt19937 engine(seed);
    engine.discard(skip);
    std::array<std::uint32_t, 640> observations{};
    for(auto& value: observations){
        value = static_cast<std::uint32_t>(engine());
    }
    Mt19937Cracker cracker(observations);
    if(!check_mt_public_api(cracker)
       || Mt19937Cracker::min() != std::mt19937::min()
       || Mt19937Cracker::max() != std::mt19937::max()){
        return 1;
    }
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted = cracker();
        const auto expected = static_cast<std::uint32_t>(engine());
        if(predicted != expected) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

int run_mt64(){
    std::uint64_t seed;
    unsigned long long skip;
    int prediction_count;
    if(!(std::cin >> seed >> skip >> prediction_count)) return 1;
    std::mt19937_64 engine(seed);
    engine.discard(skip);
    std::array<std::uint64_t, 320> observations{};
    for(auto& value: observations) value = engine();
    Mt19937_64Cracker cracker(observations);
    if(!check_mt_public_api(cracker)
       || Mt19937_64Cracker::min() != std::mt19937_64::min()
       || Mt19937_64Cracker::max() != std::mt19937_64::max()){
        return 1;
    }
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted = cracker();
        const auto expected = engine();
        if(predicted != expected) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

template<class Engine, class Cracker, std::size_t ObservationCount>
int run_subtract_with_carry(){
    using result_type = typename Engine::result_type;
    result_type seed;
    result_type seed_first;
    result_type seed_last;
    unsigned long long skip;
    int prediction_count;
    if(!(std::cin >> seed >> skip >> seed_first >> seed_last
         >> prediction_count)){
        return 1;
    }

    Engine engine(seed);
    engine.discard(skip);
    std::array<result_type, ObservationCount> observations{};
    for(auto& value: observations) value = engine();
    Cracker cracker(observations);
    const auto recovered_state = cracker.state();
    for(std::size_t index = 0; index < recovered_state.size(); ++index){
        if(recovered_state[index] != observations[index + 1]) return 1;
    }
    if(!check_swc_public_api(cracker)
       || Cracker::min() != Engine::min()
       || Cracker::max() != Engine::max()){
        return 1;
    }

    std::array<result_type, 3> prefix{
        observations[0],
        observations[1],
        observations[2],
    };
    auto recovery = recover_subtract_with_carry_seed_in_range<Engine>(
        prefix,
        skip,
        seed_first,
        seed_last
    );
    if(!recovery.unique() || recovery.seed != seed
       || recovery.recovered_seed() != seed){
        return 1;
    }
    Engine prefix_reference(seed);
    prefix_reference.discard(skip + prefix.size());
    if(recovery.next() != prefix_reference()) return 1;
    std::cout << recovery.seed << '\n';

    for(int query = 0; query < prediction_count; ++query){
        const auto predicted = cracker();
        const auto expected = engine();
        if(predicted != expected) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

template<class Engine>
int run_philox(){
    using result_type = typename Engine::result_type;
    result_type seed;
    result_type seed_first;
    result_type seed_last;
    unsigned long long skip;
    int prediction_count;
    if(!(std::cin >> seed >> skip >> seed_first >> seed_last
         >> prediction_count)){
        return 1;
    }

    Engine engine(seed);
    engine.discard(skip);
    std::array<result_type, 8> observations{};
    for(auto& value: observations) value = engine();
    auto recovery = recover_philox_seed_in_range<Engine>(
        observations,
        skip,
        seed_first,
        seed_last
    );
    if(!recovery.unique() || recovery.seed != seed) return 1;
    std::cout << static_cast<int>(recovery.status) << '\n';
    std::cout << recovery.seed << '\n';
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted = recovery.next();
        const auto expected = engine();
        if(predicted != expected) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    if(!philox_known_answers() || !deterministic_self_checks()) return 1;

    std::string mode;
    if(!(std::cin >> mode)) return 1;
    if(mode == "xorshift") return run_xorshift();
    if(mode == "lcg") return run_lcg();
    if(mode == "mt32") return run_mt32();
    if(mode == "mt64") return run_mt64();
    if(mode == "swc24"){
        return run_subtract_with_carry<
            std::ranlux24_base,
            Ranlux24BaseCracker,
            25
        >();
    }
    if(mode == "swc48"){
        return run_subtract_with_carry<
            std::ranlux48_base,
            Ranlux48BaseCracker,
            13
        >();
    }
    if(mode == "philox32") return run_philox<Philox4x32>();
    if(mode == "philox64") return run_philox<Philox4x64>();
    return 1;
}
