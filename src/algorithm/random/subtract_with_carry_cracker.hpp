#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
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
        unsigned __int128
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
