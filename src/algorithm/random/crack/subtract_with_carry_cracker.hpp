#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_SUBTRACT_WITH_CARRY_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_SUBTRACT_WITH_CARRY_CRACKER_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<
    class UInt,
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag
>
class SubtractWithCarryCracker{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);
    static_assert(0 < WordSize);
    static_assert(WordSize <= std::numeric_limits<UInt>::digits);
    static_assert(0 < ShortLag && ShortLag < LongLag);

public:
    using result_type = UInt;
    using state_type = std::array<result_type, LongLag>;

private:
    using wide_type = std::conditional_t<
        (WordSize < 64),
        std::uint64_t,
        __uint128_t
    >;

    state_type state_{};
    std::size_t head_ = 0;
    bool carry_ = false;

    static constexpr wide_type modulus() noexcept{
        return wide_type(1) << WordSize;
    }

    static constexpr result_type word_mask() noexcept{
        if constexpr(WordSize == std::numeric_limits<result_type>::digits){
            return std::numeric_limits<result_type>::max();
        }else{
            return static_cast<result_type>(
                (result_type(1) << WordSize) - 1
            );
        }
    }

    static constexpr bool valid_word(result_type value) noexcept{
        return (value & static_cast<result_type>(~word_mask())) == 0;
    }

    static constexpr result_type subtract(
        result_type short_value,
        result_type long_value,
        bool carry
    ) noexcept{
        const wide_type difference =
            static_cast<wide_type>(short_value)
            + modulus()
            - static_cast<wide_type>(long_value)
            - static_cast<unsigned>(carry);
        return static_cast<result_type>(
            difference & (modulus() - 1)
        );
    }

    static constexpr bool next_carry(
        result_type short_value,
        result_type long_value,
        bool carry
    ) noexcept{
        return static_cast<wide_type>(short_value)
            < static_cast<wide_type>(long_value)
                + static_cast<unsigned>(carry);
    }

public:
    template<std::size_t Count>
    constexpr explicit SubtractWithCarryCracker(
        const std::array<result_type, Count>& consecutive_outputs
    ){
        static_assert(
            LongLag + 1 <= Count,
            "subtract-with-carry state recovery needs LongLag + 1 outputs"
        );
        for(result_type value: consecutive_outputs){
            if(!valid_word(value)){
                throw std::invalid_argument(
                    "subtract-with-carry output exceeds word size"
                );
            }
        }

        const result_type short_value =
            consecutive_outputs[LongLag - ShortLag];
        const result_type long_value = consecutive_outputs[0];
        const result_type observed = consecutive_outputs[LongLag];
        const result_type without_carry =
            subtract(short_value, long_value, false);
        const result_type with_carry =
            subtract(short_value, long_value, true);

        bool previous_carry;
        if(observed == without_carry){
            previous_carry = false;
        }else if(observed == with_carry){
            previous_carry = true;
        }else{
            throw std::invalid_argument(
                "outputs do not belong to one subtract-with-carry stream"
            );
        }

        carry_ = next_carry(short_value, long_value, previous_carry);
        for(std::size_t index = 0; index < LongLag; ++index){
            state_[index] = consecutive_outputs[index + 1];
        }
        for(std::size_t index = LongLag + 1; index < Count; ++index){
            if(next() != consecutive_outputs[index]){
                throw std::invalid_argument(
                    "outputs do not belong to one subtract-with-carry stream"
                );
            }
        }
    }

    constexpr result_type operator()() noexcept{
        return next();
    }

    constexpr result_type next() noexcept{
        std::size_t short_index = head_ + (LongLag - ShortLag);
        if(short_index >= LongLag){
            short_index -= LongLag;
        }
        const result_type short_value = state_[short_index];
        const result_type long_value = state_[head_];
        const result_type result =
            subtract(short_value, long_value, carry_);
        carry_ = next_carry(short_value, long_value, carry_);
        state_[head_] = result;
        ++head_;
        if(head_ == LongLag) head_ = 0;
        return result;
    }

    constexpr void discard(unsigned long long count) noexcept{
        while(count-- != 0) (void)next();
    }

    constexpr bool carry() const noexcept{
        return carry_;
    }

    constexpr state_type state() const noexcept{
        state_type result{};
        std::size_t position = head_;
        for(std::size_t index = 0; index < LongLag; ++index){
            result[index] = state_[position];
            ++position;
            if(position == LongLag) position = 0;
        }
        return result;
    }

    static constexpr result_type min() noexcept{
        return 0;
    }

    static constexpr result_type max() noexcept{
        return word_mask();
    }
};

using Ranlux24BaseCracker =
    SubtractWithCarryCracker<std::uint_fast32_t, 24, 10, 24>;

using Ranlux48BaseCracker =
    SubtractWithCarryCracker<std::uint_fast64_t, 48, 5, 12>;

template<class SeedUInt>
struct StandardSubtractWithCarrySeedClass{
    static_assert(std::is_unsigned_v<SeedUInt>);
    static_assert(std::numeric_limits<SeedUInt>::digits <= 64);

    using seed_type = SeedUInt;

    static constexpr std::uint64_t lcg_modulus = 2147483563ULL;
    static constexpr std::uint64_t lcg_default_seed = 19780503ULL;

    std::uint64_t lcg_state = 0;

    constexpr bool contains(seed_type seed) const noexcept{
        std::uint64_t normalized;
        if(seed == 0){
            normalized = lcg_default_seed;
        }else{
            normalized =
                static_cast<std::uint64_t>(seed) % lcg_modulus;
            if(normalized == 0) normalized = 1;
        }
        return normalized == lcg_state;
    }

    constexpr bool contains_zero_seed() const noexcept{
        return lcg_state == lcg_default_seed;
    }

    constexpr seed_type canonical_seed() const noexcept{
        if(contains_zero_seed()) return 0;
        return static_cast<seed_type>(lcg_state);
    }
};

template<class Cracker, class SeedUInt>
struct StandardSubtractWithCarrySeedRecovery{
    using cracker_type = Cracker;
    using result_type = typename cracker_type::result_type;
    using seed_type = SeedUInt;
    using seed_class_type =
        StandardSubtractWithCarrySeedClass<seed_type>;

    static constexpr std::size_t maximum_seed_classes = 512;

    std::array<seed_class_type, maximum_seed_classes> seed_classes{};
    std::size_t seed_class_count = 0;
    std::optional<cracker_type> predictor;

    constexpr bool unique_seed_class() const noexcept{
        return seed_class_count == 1;
    }

    constexpr std::optional<seed_type> canonical_seed() const noexcept{
        if(!unique_seed_class()) return std::nullopt;
        return seed_classes[0].canonical_seed();
    }

    constexpr bool contains_seed(seed_type seed) const noexcept{
        for(std::size_t index = 0; index < seed_class_count; ++index){
            if(seed_classes[index].contains(seed)) return true;
        }
        return false;
    }

    result_type next(){
        if(!predictor){
            throw std::logic_error(
                "subtract-with-carry predictor is unavailable"
            );
        }
        return predictor->next();
    }
};

namespace subtract_with_carry_cracker_detail{

inline constexpr std::uint64_t lcg_modulus = 2147483563ULL;
inline constexpr std::uint64_t lcg_multiplier = 40014ULL;
inline constexpr std::uint64_t lcg_multiplier_inverse = 2082061899ULL;

template<std::size_t WordSize>
using wide_type = std::conditional_t<
    (WordSize < 64),
    std::uint64_t,
    __uint128_t
>;

template<std::size_t WordSize, class Result>
constexpr Result subtract_word(
    Result short_value,
    Result long_value,
    bool carry
) noexcept{
    using wide = wide_type<WordSize>;
    const wide modulus = wide(1) << WordSize;
    return static_cast<Result>(
        (static_cast<wide>(short_value)
            + modulus
            - static_cast<wide>(long_value)
            - static_cast<unsigned>(carry))
        & (modulus - 1)
    );
}

template<class Result>
constexpr bool next_carry(
    Result short_value,
    Result long_value,
    bool carry
) noexcept{
    return static_cast<__uint128_t>(short_value)
        < static_cast<__uint128_t>(long_value)
            + static_cast<unsigned>(carry);
}

template<class Result, std::size_t LongLag>
struct InitialStateSet{
    std::array<std::array<Result, LongLag>, 4> values{};
    std::size_t count = 0;
};

template<
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag,
    class Result,
    std::size_t Count
>
constexpr bool state_matches(
    std::array<Result, LongLag> state,
    const std::array<Result, Count>& outputs
) noexcept{
    bool carry = state[LongLag - 1] == 0;
    std::size_t head = 0;
    for(Result observed: outputs){
        std::size_t short_index =
            head + (LongLag - ShortLag);
        if(short_index >= LongLag) short_index -= LongLag;
        const Result short_value = state[short_index];
        const Result long_value = state[head];
        const Result generated = subtract_word<WordSize>(
            short_value,
            long_value,
            carry
        );
        if(generated != observed) return false;
        carry = next_carry(short_value, long_value, carry);
        state[head] = generated;
        ++head;
        if(head == LongLag) head = 0;
    }
    return true;
}

template<
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag,
    class Result,
    std::size_t Count
>
constexpr InitialStateSet<Result, LongLag> recover_initial_states(
    const std::array<Result, Count>& outputs
){
    static_assert(LongLag <= Count);
    static_assert(
        2 * ShortLag <= LongLag,
        "direct standard-seed recovery requires 2 * ShortLag <= LongLag"
    );

    InitialStateSet<Result, LongLag> result;
    for(unsigned carry_at_short_lag = 0;
        carry_at_short_lag < 2;
        ++carry_at_short_lag){
        std::array<Result, LongLag> high_state{};
        bool carry = carry_at_short_lag != 0;
        for(std::size_t index = ShortLag;
            index < LongLag;
            ++index){
            const Result short_value = outputs[index - ShortLag];
            const Result long_value = subtract_word<WordSize>(
                short_value,
                outputs[index],
                carry
            );
            high_state[index] = long_value;
            carry = next_carry(short_value, long_value, carry);
        }

        for(unsigned initial_carry = 0;
            initial_carry < 2;
            ++initial_carry){
            auto state = high_state;
            carry = initial_carry != 0;
            for(std::size_t index = 0;
                index < ShortLag;
                ++index){
                const Result short_value =
                    state[LongLag - ShortLag + index];
                const Result long_value = subtract_word<WordSize>(
                    short_value,
                    outputs[index],
                    carry
                );
                state[index] = long_value;
                carry = next_carry(short_value, long_value, carry);
            }
            if(carry != (carry_at_short_lag != 0)) continue;
            if((state[LongLag - 1] == 0)
               != (initial_carry != 0)){
                continue;
            }
            if(!state_matches<
                WordSize,
                ShortLag,
                LongLag
            >(state, outputs)){
                continue;
            }

            bool duplicate = false;
            for(std::size_t index = 0; index < result.count; ++index){
                if(result.values[index] == state){
                    duplicate = true;
                    break;
                }
            }
            if(!duplicate){
                result.values[result.count++] = state;
            }
        }
    }
    return result;
}

template<
    std::size_t WordSize,
    std::size_t LongLag,
    class Result
>
constexpr std::array<Result, LongLag> make_seeded_state(
    std::uint64_t lcg_state
) noexcept{
    using wide = wide_type<WordSize>;
    const wide mask = (wide(1) << WordSize) - 1;
    constexpr std::size_t pieces = (WordSize + 31) / 32;
    std::array<Result, LongLag> state{};
    for(Result& value: state){
        wide combined = 0;
        for(std::size_t piece = 0; piece < pieces; ++piece){
            lcg_state =
                lcg_state * lcg_multiplier % lcg_modulus;
            combined |=
                static_cast<wide>(lcg_state) << (32 * piece);
        }
        value = static_cast<Result>(combined & mask);
    }
    return state;
}

template<
    std::size_t WordSize,
    std::size_t LongLag,
    class Recovery
>
constexpr void append_seed_classes(
    const std::array<typename Recovery::result_type, LongLag>& state,
    Recovery& recovery
) noexcept{
    using result_type = typename Recovery::result_type;
    const auto try_first_lcg_output = [&](std::uint64_t first){
        if(first == 0 || first >= lcg_modulus) return;
        const std::uint64_t lcg_state =
            first * lcg_multiplier_inverse % lcg_modulus;
        if(make_seeded_state<
            WordSize,
            LongLag,
            result_type
        >(lcg_state) != state){
            return;
        }
        for(std::size_t index = 0;
            index < recovery.seed_class_count;
            ++index){
            if(recovery.seed_classes[index].lcg_state == lcg_state){
                return;
            }
        }
        recovery.seed_classes[recovery.seed_class_count++].lcg_state =
            lcg_state;
    };

    if constexpr(WordSize < 32){
        constexpr std::uint64_t step =
            std::uint64_t(1) << WordSize;
        std::uint64_t first = static_cast<std::uint64_t>(state[0]);
        while(first < lcg_modulus){
            try_first_lcg_output(first);
            if(first > lcg_modulus - 1 - step) break;
            first += step;
        }
    }else{
        try_first_lcg_output(
            static_cast<std::uint64_t>(state[0]) & 0xffffffffULL
        );
    }
}

template<
    class Engine,
    class Cracker,
    std::size_t WordSize,
    std::size_t ShortLag,
    std::size_t LongLag,
    std::size_t Count
>
auto recover_standard_seed(
    const std::array<typename Engine::result_type, Count>& outputs
){
    static_assert(LongLag + 1 <= Count);
    using recovery_type = StandardSubtractWithCarrySeedRecovery<
        Cracker,
        typename Engine::result_type
    >;

    recovery_type recovery;
    recovery.predictor.emplace(outputs);
    const auto states = recover_initial_states<
        WordSize,
        ShortLag,
        LongLag
    >(outputs);
    for(std::size_t index = 0; index < states.count; ++index){
        append_seed_classes<WordSize, LongLag>(
            states.values[index],
            recovery
        );
    }
    return recovery;
}

} // namespace subtract_with_carry_cracker_detail

using Ranlux24BaseStandardSeedRecovery =
    StandardSubtractWithCarrySeedRecovery<
        Ranlux24BaseCracker,
        std::ranlux24_base::result_type
    >;

using Ranlux48BaseStandardSeedRecovery =
    StandardSubtractWithCarrySeedRecovery<
        Ranlux48BaseCracker,
        std::ranlux48_base::result_type
    >;

template<std::size_t Count>
Ranlux24BaseStandardSeedRecovery
recover_ranlux24_base_standard_seed(
    const std::array<std::ranlux24_base::result_type, Count>& outputs
){
    return subtract_with_carry_cracker_detail::recover_standard_seed<
        std::ranlux24_base,
        Ranlux24BaseCracker,
        24,
        10,
        24
    >(outputs);
}

template<std::size_t Count>
Ranlux48BaseStandardSeedRecovery
recover_ranlux48_base_standard_seed(
    const std::array<std::ranlux48_base::result_type, Count>& outputs
){
    return subtract_with_carry_cracker_detail::recover_standard_seed<
        std::ranlux48_base,
        Ranlux48BaseCracker,
        48,
        5,
        12
    >(outputs);
}

enum class SubtractWithCarrySeedRecoveryStatus{
    none,
    unique,
    ambiguous,
};

template<class Engine>
struct SubtractWithCarrySeedRecovery{
    using result_type = typename Engine::result_type;

    SubtractWithCarrySeedRecoveryStatus status =
        SubtractWithCarrySeedRecoveryStatus::none;
    result_type seed{};
    Engine predictor{};

    constexpr bool unique() const noexcept{
        return status == SubtractWithCarrySeedRecoveryStatus::unique;
    }

    constexpr std::optional<result_type> recovered_seed() const noexcept{
        if(!unique()) return std::nullopt;
        return seed;
    }

    result_type next(){
        if(!unique()){
            throw std::logic_error(
                "subtract-with-carry seed recovery is not unique"
            );
        }
        return predictor();
    }
};

template<class Engine, std::size_t Count>
SubtractWithCarrySeedRecovery<Engine>
recover_subtract_with_carry_seed_candidates(
    const std::array<typename Engine::result_type, Count>& consecutive_outputs,
    unsigned long long discard_count,
    typename Engine::result_type seed_first,
    unsigned long long candidate_count
){
    static_assert(0 < Count);
    using result_type = typename Engine::result_type;
    static_assert(std::is_unsigned_v<result_type>);
    static_assert(
        std::numeric_limits<result_type>::digits
            <= std::numeric_limits<unsigned long long>::digits
    );
    if(
        candidate_count != 0
        && candidate_count - 1 > static_cast<unsigned long long>(
            std::numeric_limits<result_type>::max() - seed_first
        )
    ){
        throw std::invalid_argument("seed candidate range overflows");
    }
    SubtractWithCarrySeedRecovery<Engine> recovery;
    result_type seed = seed_first;
    for(
        unsigned long long offset = 0;
        offset < candidate_count;
        ++offset
    ){
        Engine engine(seed);
        engine.discard(discard_count);
        bool matches = true;
        for(result_type observed: consecutive_outputs){
            if(engine() != observed){
                matches = false;
                break;
            }
        }
        if(matches){
            if(recovery.status == SubtractWithCarrySeedRecoveryStatus::none){
                recovery.status = SubtractWithCarrySeedRecoveryStatus::unique;
                recovery.seed = seed;
                recovery.predictor = std::move(engine);
            }else{
                recovery.status =
                    SubtractWithCarrySeedRecoveryStatus::ambiguous;
                return recovery;
            }
        }
        if(offset + 1 != candidate_count) ++seed;
    }
    return recovery;
}

template<class Engine, std::size_t Count>
SubtractWithCarrySeedRecovery<Engine>
recover_subtract_with_carry_seed_in_range(
    const std::array<typename Engine::result_type, Count>& consecutive_outputs,
    unsigned long long discard_count,
    typename Engine::result_type seed_first,
    typename Engine::result_type seed_last
){
    static_assert(0 < Count);
    using result_type = typename Engine::result_type;
    static_assert(std::is_unsigned_v<result_type>);
    static_assert(
        std::numeric_limits<result_type>::digits
            <= std::numeric_limits<unsigned long long>::digits
    );
    if(seed_last < seed_first){
        throw std::invalid_argument("invalid seed range");
    }
    return recover_subtract_with_carry_seed_candidates<Engine>(
        consecutive_outputs,
        discard_count,
        seed_first,
        static_cast<unsigned long long>(seed_last - seed_first)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_SUBTRACT_WITH_CARRY_CRACKER_HPP_INCLUDED
