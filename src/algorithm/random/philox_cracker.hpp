#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
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
PhiloxSeedRecovery<Engine> recover_philox_seed_in_range(
    const std::array<typename Engine::result_type, Count>& consecutive_outputs,
    unsigned long long discard_count,
    typename Engine::result_type seed_first,
    typename Engine::result_type seed_last
){
    static_assert(0 < Count);
    using result_type = typename Engine::result_type;
    if(seed_last < seed_first){
        throw std::invalid_argument("invalid seed range");
    }

    PhiloxSeedRecovery<Engine> recovery;
    for(result_type seed = seed_first; seed != seed_last; ++seed){
        Engine engine(seed);
        engine.discard(discard_count);
        bool matches = true;
        for(result_type observed: consecutive_outputs){
            if(engine() != observed){
                matches = false;
                break;
            }
        }
        if(!matches) continue;

        if(recovery.status == PhiloxSeedRecoveryStatus::none){
            recovery.status = PhiloxSeedRecoveryStatus::unique;
            recovery.seed = seed;
            recovery.predictor = std::move(engine);
        }else{
            recovery.status = PhiloxSeedRecoveryStatus::ambiguous;
            return recovery;
        }
    }
    return recovery;
}
