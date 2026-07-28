#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_DIGIT_DIGIT_DP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_DIGIT_DIGIT_DP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dynamic_programming {

namespace digit_dp_internal {

template<class State>
struct StateAndStarted {
    State state;
    bool started = false;

    bool operator==(const StateAndStarted&) const = default;
};

template<class State, class Hash>
struct StateAndStartedHash {
    Hash hash;

    std::size_t operator()(const StateAndStarted<State>& value) const {
        const std::size_t state_hash = hash(value.state);
        return state_hash
            ^ (static_cast<std::size_t>(value.started)
               + 0x9e3779b97f4a7c15ULL
               + (state_hash << 6) + (state_hash >> 2));
    }
};

inline void validate_digits(
    const std::vector<int>& digits,
    int base
) {
    if(base < 2){
        throw std::invalid_argument("digit DP base must be at least 2");
    }
    if(digits.empty()){
        throw std::invalid_argument("digit DP needs at least one digit");
    }
    for(const int digit : digits){
        if(digit < 0 || digit >= base){
            throw std::invalid_argument("digit is outside the base");
        }
    }
}

inline std::optional<std::vector<int>> decrement_digits(
    std::vector<int> digits,
    int base
) {
    for(std::size_t offset = 0; offset < digits.size(); ++offset){
        const std::size_t index = digits.size() - 1 - offset;
        if(digits[index] != 0){
            --digits[index];
            return digits;
        }
        digits[index] = base - 1;
    }
    return std::nullopt;
}

}  // namespace digit_dp_internal

template<class Count, class State, class Transition, class Accept,
         class Hash = std::hash<State>,
         class Equal = std::equal_to<State>>
Count digit_dp_count(
    const std::vector<int>& upper_digits,
    int base,
    State initial_state,
    Transition transition,
    Accept accept,
    Hash hash = {},
    Equal equal = {}
) {
    digit_dp_internal::validate_digits(upper_digits, base);
    using Key = digit_dp_internal::StateAndStarted<State>;
    using KeyHash = digit_dp_internal::StateAndStartedHash<State, Hash>;
    struct KeyEqual {
        Equal equal;
        bool operator()(const Key& first, const Key& second) const {
            return first.started == second.started
                && equal(first.state, second.state);
        }
    };
    using Map = std::unordered_map<Key, Count, KeyHash, KeyEqual>;
    Map tight(0, KeyHash{hash}, KeyEqual{equal});
    Map loose(0, KeyHash{hash}, KeyEqual{equal});
    tight.emplace(Key{std::move(initial_state), false}, Count{1});

    for(std::size_t position = 0;
        position < upper_digits.size();
        ++position){
        Map next_tight(0, KeyHash{hash}, KeyEqual{equal});
        Map next_loose(0, KeyHash{hash}, KeyEqual{equal});
        const auto extend = [&](const Map& source, bool source_tight){
            for(const auto& [key, ways] : source){
                const int maximum =
                    source_tight ? upper_digits[position] : base - 1;
                for(int digit = 0; digit <= maximum; ++digit){
                    const bool next_started = key.started || digit != 0;
                    std::optional<State> next_state = std::invoke(
                        transition,
                        key.state,
                        digit,
                        key.started,
                        next_started,
                        position
                    );
                    if(!next_state) continue;
                    const bool next_is_tight =
                        source_tight && digit == maximum;
                    Map& destination =
                        next_is_tight ? next_tight : next_loose;
                    destination[
                        Key{std::move(*next_state), next_started}
                    ] += ways;
                }
            }
        };
        extend(tight, true);
        extend(loose, false);
        tight = std::move(next_tight);
        loose = std::move(next_loose);
    }

    Count result{};
    const auto collect = [&](const Map& states){
        for(const auto& [key, ways] : states){
            if(static_cast<bool>(std::invoke(
                accept, key.state, key.started
            ))){
                result += ways;
            }
        }
    };
    collect(tight);
    collect(loose);
    return result;
}

template<class Count, class State, class Transition, class Accept,
         class Hash = std::hash<State>,
         class Equal = std::equal_to<State>>
Count digit_dp_count_range(
    const std::vector<int>& lower_digits,
    const std::vector<int>& upper_digits,
    int base,
    const State& initial_state,
    Transition transition,
    Accept accept,
    Hash hash = {},
    Equal equal = {}
) {
    digit_dp_internal::validate_digits(lower_digits, base);
    digit_dp_internal::validate_digits(upper_digits, base);
    if(lower_digits.size() != upper_digits.size()
       || lower_digits > upper_digits){
        throw std::invalid_argument("invalid digit DP range");
    }
    const Count upper = digit_dp_count<Count>(
        upper_digits, base, initial_state, transition, accept, hash, equal
    );
    const auto before_lower =
        digit_dp_internal::decrement_digits(lower_digits, base);
    if(!before_lower) return upper;
    return upper - digit_dp_count<Count>(
        *before_lower, base, initial_state, transition, accept, hash, equal
    );
}

inline std::vector<int> decimal_digits(const std::string& value) {
    if(value.empty()){
        throw std::invalid_argument("empty decimal digit string");
    }
    std::vector<int> result;
    result.reserve(value.size());
    for(const char character : value){
        if(character < '0' || character > '9'){
            throw std::invalid_argument("non-decimal character");
        }
        result.push_back(character - '0');
    }
    return result;
}

}  // namespace dynamic_programming

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_DIGIT_DIGIT_DP_HPP_INCLUDED
