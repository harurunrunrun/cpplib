#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "philox_engine.hpp"

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
