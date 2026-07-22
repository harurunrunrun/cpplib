#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_PHILOX_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_PHILOX_CRACKER_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "philox_engine.hpp"

namespace philox_cracker_detail{

template<class Engine>
using wide_type = std::conditional_t<
    (Engine::word_size <= 32),
    std::uint64_t,
    __uint128_t
>;

template<class Engine>
constexpr wide_type<Engine> word_mask() noexcept{
    return (wide_type<Engine>(1) << Engine::word_size) - 1;
}

template<class Engine>
constexpr typename Engine::result_type mask_word(
    typename Engine::result_type value
) noexcept{
    return static_cast<typename Engine::result_type>(
        static_cast<wide_type<Engine>>(value) & word_mask<Engine>()
    );
}

template<class Engine>
constexpr bool valid_word(
    typename Engine::result_type value
) noexcept{
    return mask_word<Engine>(value) == value;
}

template<class Engine, class Array>
constexpr bool valid_words(const Array& values) noexcept{
    for(const auto value: values){
        if(!valid_word<Engine>(value)) return false;
    }
    return true;
}

template<class Engine>
constexpr std::pair<
    typename Engine::result_type,
    typename Engine::result_type
> multiply_halves(
    typename Engine::result_type left,
    typename Engine::result_type right
) noexcept{
    using result_type = typename Engine::result_type;
    const wide_type<Engine> product =
        static_cast<wide_type<Engine>>(mask_word<Engine>(left))
        * mask_word<Engine>(right);
    return {
        static_cast<result_type>(
            (product >> Engine::word_size) & word_mask<Engine>()
        ),
        static_cast<result_type>(product & word_mask<Engine>()),
    };
}

template<class Engine>
constexpr typename Engine::block_type permute(
    const typename Engine::block_type& value
) noexcept{
    if constexpr(Engine::word_count == 2){
        return value;
    }else{
        return {value[2], value[1], value[0], value[3]};
    }
}

template<class Engine>
constexpr typename Engine::block_type counter_to_internal(
    const typename Engine::counter_type& counter
) noexcept{
    typename Engine::block_type result{};
    for(std::size_t index = 0; index < Engine::word_count; ++index){
        result[index] = counter[Engine::word_count - 1 - index];
    }
    return result;
}

template<class Engine>
constexpr typename Engine::counter_type internal_to_counter(
    const typename Engine::block_type& internal
) noexcept{
    typename Engine::counter_type result{};
    for(std::size_t index = 0; index < Engine::word_count; ++index){
        result[index] = internal[Engine::word_count - 1 - index];
    }
    return result;
}

template<class Engine>
constexpr typename Engine::block_type generate_block(
    const typename Engine::counter_type& counter,
    const typename Engine::key_type& key
) noexcept{
    using result_type = typename Engine::result_type;
    typename Engine::block_type current =
        counter_to_internal<Engine>(counter);
    for(std::size_t round = 0; round < Engine::round_count; ++round){
        const auto shuffled = permute<Engine>(current);
        typename Engine::block_type next{};
        for(std::size_t index = 0;
            index < Engine::word_count / 2;
            ++index){
            const auto [high, low] = multiply_halves<Engine>(
                shuffled[2 * index],
                Engine::multipliers[index]
            );
            const result_type round_key = static_cast<result_type>(
                (static_cast<wide_type<Engine>>(key[index])
                    + static_cast<wide_type<Engine>>(round)
                        * Engine::round_consts[index])
                & word_mask<Engine>()
            );
            next[2 * index] = mask_word<Engine>(
                high ^ round_key ^ shuffled[2 * index + 1]
            );
            next[2 * index + 1] = low;
        }
        current = next;
    }
    return current;
}

template<class Engine>
constexpr typename Engine::result_type odd_inverse(
    typename Engine::result_type value
) noexcept{
    using result_type = typename Engine::result_type;
    wide_type<Engine> inverse = 1;
    const wide_type<Engine> multiplier = mask_word<Engine>(value);
    for(std::size_t correct_bits = 1;
        correct_bits < Engine::word_size;
        correct_bits <<= 1){
        inverse =
            inverse * (2 - multiplier * inverse)
            & word_mask<Engine>();
    }
    return static_cast<result_type>(inverse);
}

template<class Engine>
typename Engine::counter_type invert_block(
    typename Engine::block_type current,
    const typename Engine::key_type& key
){
    using result_type = typename Engine::result_type;
    for(std::size_t index = 0;
        index < Engine::word_count / 2;
        ++index){
        if((Engine::multipliers[index] & 1U) == 0U){
            throw std::invalid_argument(
                "Philox block inversion requires odd multipliers"
            );
        }
    }

    for(std::size_t round = Engine::round_count;
        round-- > 0;){
        typename Engine::block_type shuffled{};
        for(std::size_t index = 0;
            index < Engine::word_count / 2;
            ++index){
            const result_type even = mask_word<Engine>(
                static_cast<result_type>(
                    static_cast<wide_type<Engine>>(
                        current[2 * index + 1]
                    ) * odd_inverse<Engine>(
                        Engine::multipliers[index]
                    )
                )
            );
            const auto [high, low] = multiply_halves<Engine>(
                even,
                Engine::multipliers[index]
            );
            if(low != current[2 * index + 1]){
                throw std::invalid_argument(
                    "Philox multiplier is not invertible"
                );
            }
            const result_type round_key = static_cast<result_type>(
                (static_cast<wide_type<Engine>>(key[index])
                    + static_cast<wide_type<Engine>>(round)
                        * Engine::round_consts[index])
                & word_mask<Engine>()
            );
            shuffled[2 * index] = even;
            shuffled[2 * index + 1] = mask_word<Engine>(
                current[2 * index] ^ high ^ round_key
            );
        }
        current = permute<Engine>(shuffled);
    }
    return internal_to_counter<Engine>(current);
}

template<class Engine>
constexpr void increment_counter(
    typename Engine::counter_type& counter
) noexcept{
    for(std::size_t index = Engine::word_count; index-- > 0;){
        if(counter[index] != Engine::max()){
            ++counter[index];
            return;
        }
        counter[index] = 0;
    }
}

template<class Engine>
constexpr void decrement_counter(
    typename Engine::counter_type& counter
) noexcept{
    for(std::size_t index = Engine::word_count; index-- > 0;){
        if(counter[index] != 0){
            --counter[index];
            return;
        }
        counter[index] = Engine::max();
    }
}

} // namespace philox_cracker_detail

template<class Engine>
constexpr typename Engine::block_type philox_generate_block(
    const typename Engine::counter_type& counter,
    const typename Engine::key_type& key
){
    if(!philox_cracker_detail::valid_words<Engine>(counter)
       || !philox_cracker_detail::valid_words<Engine>(key)){
        throw std::invalid_argument(
            "Philox counter or key exceeds word size"
        );
    }
    return philox_cracker_detail::generate_block<Engine>(counter, key);
}

template<class Engine>
typename Engine::counter_type philox_invert_block(
    const typename Engine::block_type& output,
    const typename Engine::key_type& key
){
    if(!philox_cracker_detail::valid_words<Engine>(output)
       || !philox_cracker_detail::valid_words<Engine>(key)){
        throw std::invalid_argument(
            "Philox output or key exceeds word size"
        );
    }
    return philox_cracker_detail::invert_block<Engine>(output, key);
}

template<class Engine>
class PhiloxKnownKeyCracker{
public:
    using engine_type = Engine;
    using result_type = typename engine_type::result_type;
    using counter_type = typename engine_type::counter_type;
    using key_type = typename engine_type::key_type;
    using block_type = typename engine_type::block_type;

private:
    key_type key_{};
    counter_type next_counter_{};
    block_type buffer_{};
    std::size_t index_ = engine_type::word_count;

public:
    constexpr PhiloxKnownKeyCracker() = default;

    constexpr PhiloxKnownKeyCracker(
        const key_type& key,
        const counter_type& first_counter,
        std::size_t first_index
    )
        : key_(key),
          next_counter_(first_counter),
          index_(first_index){
        if(first_index >= engine_type::word_count){
            throw std::out_of_range(
                "Philox block index is out of range"
            );
        }
        if(!philox_cracker_detail::valid_words<engine_type>(key)
           || !philox_cracker_detail::valid_words<engine_type>(
               first_counter
           )){
            throw std::invalid_argument(
                "Philox counter or key exceeds word size"
            );
        }
        buffer_ = philox_cracker_detail::generate_block<engine_type>(
            next_counter_,
            key_
        );
        philox_cracker_detail::increment_counter<engine_type>(
            next_counter_
        );
    }

    constexpr result_type next() noexcept{
        if(index_ == engine_type::word_count){
            buffer_ = philox_cracker_detail::generate_block<engine_type>(
                next_counter_,
                key_
            );
            philox_cracker_detail::increment_counter<engine_type>(
                next_counter_
            );
            index_ = 0;
        }
        return buffer_[index_++];
    }

    constexpr result_type operator()() noexcept{
        return next();
    }

    constexpr const key_type& key() const noexcept{
        return key_;
    }

    constexpr const counter_type& next_block_counter() const noexcept{
        return next_counter_;
    }

    constexpr std::size_t buffer_index() const noexcept{
        return index_;
    }
};

template<class Engine>
struct PhiloxKnownKeyCandidate{
    using counter_type = typename Engine::counter_type;
    using predictor_type = PhiloxKnownKeyCracker<Engine>;

    counter_type first_counter{};
    std::size_t first_index = 0;
    predictor_type predictor{};
};

template<class Engine>
struct PhiloxKnownKeyRecovery{
    using engine_type = Engine;
    using result_type = typename engine_type::result_type;
    using candidate_type = PhiloxKnownKeyCandidate<engine_type>;

    static constexpr std::size_t maximum_candidates =
        engine_type::word_count;
    static constexpr std::size_t minimum_observation_count =
        3 * engine_type::word_count - 1;

    std::array<candidate_type, maximum_candidates> candidates{};
    std::size_t candidate_count = 0;

    constexpr bool unique() const noexcept{
        return candidate_count == 1;
    }

    result_type next(){
        if(!unique()){
            throw std::logic_error(
                "Philox counter/alignment recovery is not unique"
            );
        }
        return candidates[0].predictor.next();
    }
};

template<class Engine, std::size_t Count>
PhiloxKnownKeyRecovery<Engine> recover_philox_known_key_stream(
    const std::array<typename Engine::result_type, Count>& outputs,
    const typename Engine::key_type& key
){
    static_assert(
        PhiloxKnownKeyRecovery<Engine>::minimum_observation_count
            <= Count,
        "Philox known-key recovery needs 3 * word_count - 1 outputs"
    );
    if(!philox_cracker_detail::valid_words<Engine>(outputs)
       || !philox_cracker_detail::valid_words<Engine>(key)){
        throw std::invalid_argument(
            "Philox output or key exceeds word size"
        );
    }

    PhiloxKnownKeyRecovery<Engine> recovery;
    for(std::size_t first_index = 0;
        first_index < Engine::word_count;
        ++first_index){
        const std::size_t first_full =
            first_index == 0 ? 0 : Engine::word_count - first_index;
        typename Engine::block_type first_block{};
        typename Engine::block_type second_block{};
        for(std::size_t index = 0;
            index < Engine::word_count;
            ++index){
            first_block[index] = outputs[first_full + index];
            second_block[index] =
                outputs[first_full + Engine::word_count + index];
        }

        auto first_full_counter =
            philox_cracker_detail::invert_block<Engine>(
                first_block,
                key
            );
        const auto second_full_counter =
            philox_cracker_detail::invert_block<Engine>(
                second_block,
                key
            );
        auto expected_second = first_full_counter;
        philox_cracker_detail::increment_counter<Engine>(
            expected_second
        );
        if(expected_second != second_full_counter) continue;

        auto first_counter = first_full_counter;
        if(first_index != 0){
            philox_cracker_detail::decrement_counter<Engine>(
                first_counter
            );
        }
        PhiloxKnownKeyCracker<Engine> predictor(
            key,
            first_counter,
            first_index
        );
        bool matches = true;
        for(const auto observed: outputs){
            if(predictor.next() != observed){
                matches = false;
                break;
            }
        }
        if(!matches) continue;

        auto& candidate =
            recovery.candidates[recovery.candidate_count++];
        candidate.first_counter = first_counter;
        candidate.first_index = first_index;
        candidate.predictor = std::move(predictor);
    }
    return recovery;
}

template<class Engine>
struct PhiloxIntegerSeedClass{
    using result_type = typename Engine::result_type;

    result_type masked_seed{};

    constexpr bool contains(result_type seed) const noexcept{
        return philox_cracker_detail::mask_word<Engine>(seed)
            == masked_seed;
    }

    constexpr result_type canonical_seed() const noexcept{
        return masked_seed;
    }
};

template<class Engine>
std::optional<typename Engine::key_type>
recover_one_round_philox_key(
    const typename Engine::counter_type& counter,
    const typename Engine::block_type& output
){
    static_assert(
        Engine::round_count == 1,
        "direct unknown-key inversion is available for one round"
    );
    if(!philox_cracker_detail::valid_words<Engine>(counter)
       || !philox_cracker_detail::valid_words<Engine>(output)){
        throw std::invalid_argument(
            "Philox counter or output exceeds word size"
        );
    }

    const auto internal =
        philox_cracker_detail::counter_to_internal<Engine>(counter);
    const auto shuffled =
        philox_cracker_detail::permute<Engine>(internal);
    typename Engine::key_type key{};
    for(std::size_t index = 0;
        index < Engine::word_count / 2;
        ++index){
        const auto [high, low] =
            philox_cracker_detail::multiply_halves<Engine>(
                shuffled[2 * index],
                Engine::multipliers[index]
            );
        if(low != output[2 * index + 1]) return std::nullopt;
        key[index] = philox_cracker_detail::mask_word<Engine>(
            output[2 * index]
            ^ high
            ^ shuffled[2 * index + 1]
        );
    }
    if(philox_cracker_detail::generate_block<Engine>(counter, key)
       != output){
        return std::nullopt;
    }
    return key;
}

template<class Engine>
std::optional<PhiloxIntegerSeedClass<Engine>>
recover_one_round_philox_integer_seed(
    const typename Engine::counter_type& counter,
    const typename Engine::block_type& output
){
    const auto key = recover_one_round_philox_key<Engine>(
        counter,
        output
    );
    if(!key) return std::nullopt;
    for(std::size_t index = 1; index < key->size(); ++index){
        if((*key)[index] != 0) return std::nullopt;
    }
    return PhiloxIntegerSeedClass<Engine>{(*key)[0]};
}

template<class Engine, std::size_t Count>
PhiloxKnownKeyRecovery<Engine> recover_philox_integer_seeded_stream(
    const std::array<typename Engine::result_type, Count>& outputs,
    typename Engine::result_type seed
){
    typename Engine::key_type key{};
    key[0] = philox_cracker_detail::mask_word<Engine>(seed);
    return recover_philox_known_key_stream<Engine>(outputs, key);
}

enum class PhiloxSeedRecoveryStatus{
    none,
    unique,
    ambiguous,
};

template<class Engine>
struct PhiloxSeedRecovery{
    using result_type = typename Engine::result_type;

    PhiloxSeedRecoveryStatus status = PhiloxSeedRecoveryStatus::none;
    result_type seed{};
    Engine predictor{};

    constexpr bool unique() const noexcept{
        return status == PhiloxSeedRecoveryStatus::unique;
    }

    constexpr std::optional<result_type> recovered_seed() const noexcept{
        if(!unique()) return std::nullopt;
        return seed;
    }

    result_type next(){
        if(!unique()){
            throw std::logic_error(
                "Philox seed recovery is not unique"
            );
        }
        return predictor();
    }
};

template<class Engine, std::size_t Count>
PhiloxSeedRecovery<Engine> recover_philox_seed_candidates(
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

    const auto available_after_first = static_cast<unsigned long long>(
        std::numeric_limits<result_type>::max() - seed_first
    );
    if(candidate_count != 0
       && candidate_count - 1 > available_after_first){
        throw std::invalid_argument("Philox seed candidate range wraps");
    }

    PhiloxSeedRecovery<Engine> recovery;
    result_type seed = seed_first;
    for(unsigned long long offset = 0;
        offset < candidate_count;
        ++offset){
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
            if(recovery.status == PhiloxSeedRecoveryStatus::none){
                recovery.status = PhiloxSeedRecoveryStatus::unique;
                recovery.seed = seed;
                recovery.predictor = std::move(engine);
            }else{
                recovery.status = PhiloxSeedRecoveryStatus::ambiguous;
                return recovery;
            }
        }
        if(offset + 1 < candidate_count) ++seed;
    }
    return recovery;
}

template<class Engine, std::size_t Count>
PhiloxSeedRecovery<Engine> recover_philox_seed_in_range(
    const std::array<typename Engine::result_type, Count>& consecutive_outputs,
    unsigned long long discard_count,
    typename Engine::result_type seed_first,
    typename Engine::result_type seed_last
){
    using result_type = typename Engine::result_type;
    static_assert(std::is_unsigned_v<result_type>);
    static_assert(
        std::numeric_limits<result_type>::digits
        <= std::numeric_limits<unsigned long long>::digits
    );
    if(seed_last < seed_first){
        throw std::invalid_argument("invalid seed range");
    }
    const auto candidate_count = static_cast<unsigned long long>(
        seed_last - seed_first
    );
    return recover_philox_seed_candidates<Engine>(
        consecutive_outputs,
        discard_count,
        seed_first,
        candidate_count
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_PHILOX_CRACKER_HPP_INCLUDED
