#ifndef CPPLIB_SRC_APPROXIMATE_SET_COVER_ADVANCED_SET_COVER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SET_COVER_ADVANCED_SET_COVER_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/set_system.hpp"
#include "greedy_set_cover.hpp"

namespace approximate::set_cover {

struct PrimalDualSetCoverResult {
    SetCoverResult cover;
    std::vector<long double> element_dual;
};

namespace advanced_set_cover_internal {

template<class Weight>
std::vector<long double> checked_weights(
    const std::vector<Weight>& weights,
    std::size_t set_count
) {
    static_assert(
        std::is_arithmetic_v<Weight> && !std::is_same_v<Weight, bool>
    );
    if(weights.size() != set_count){
        throw std::invalid_argument(
            "the number of weights must equal the number of sets"
        );
    }
    std::vector<long double> result(set_count);
    for(std::size_t index = 0; index < set_count; ++index){
        result[index] = static_cast<long double>(weights[index]);
        if(result[index] < 0.0L || !std::isfinite(result[index])){
            throw std::invalid_argument(
                "set weights must be finite and nonnegative"
            );
        }
    }
    return result;
}

inline SetCoverResult reverse_delete(
    const detail::SetSystem& system,
    std::vector<std::size_t> selected
) {
    std::vector<std::size_t> cover_count(system.incidence.size(), 0);
    for(const std::size_t set_index : selected){
        for(const std::size_t element : system.sets[set_index]){
            ++cover_count[element];
        }
    }
    std::vector<unsigned char> keep(selected.size(), 1);
    for(std::size_t position = selected.size(); position > 0; --position){
        const std::size_t index = position - 1;
        const std::size_t set_index = selected[index];
        bool removable = true;
        for(const std::size_t element : system.sets[set_index]){
            if(cover_count[element] <= 1){
                removable = false;
                break;
            }
        }
        if(!removable) continue;
        keep[index] = 0;
        for(const std::size_t element : system.sets[set_index]){
            --cover_count[element];
        }
    }
    SetCoverResult result;
    for(std::size_t position = 0; position < selected.size(); ++position){
        if(keep[position] != 0){
            result.selected_sets.push_back(selected[position]);
        }
    }
    for(std::size_t element = 0; element < cover_count.size(); ++element){
        if(cover_count[element] == 0){
            result.uncovered_elements.push_back(element);
        }
    }
    return result;
}

inline std::vector<long double> checked_fractional_solution(
    std::size_t universe_size,
    const detail::SetSystem& system,
    const std::vector<long double>& fractional,
    bool require_feasible
) {
    if(fractional.size() != system.sets.size()){
        throw std::invalid_argument(
            "fractional solution has a different set count"
        );
    }
    for(const long double value : fractional){
        if(value < 0.0L || value > 1.0L || !std::isfinite(value)){
            throw std::invalid_argument(
                "fractional values must be finite and in [0, 1]"
            );
        }
    }
    if(require_feasible){
        for(std::size_t element = 0; element < universe_size; ++element){
            long double sum = 0.0L;
            for(const std::size_t set_index : system.incidence[element]){
                sum += fractional[set_index];
            }
            if(sum + 64.0L * std::numeric_limits<long double>::epsilon()
               < 1.0L){
                throw std::invalid_argument(
                    "fractional set-cover solution is infeasible"
                );
            }
        }
    }
    return fractional;
}

inline SetCoverResult selected_to_result(
    const detail::SetSystem& system,
    const std::vector<unsigned char>& selected
) {
    std::vector<bool> covered(system.incidence.size(), false);
    SetCoverResult result;
    for(std::size_t set_index = 0; set_index < selected.size(); ++set_index){
        if(selected[set_index] == 0) continue;
        result.selected_sets.push_back(set_index);
        for(const std::size_t element : system.sets[set_index]){
            covered[element] = true;
        }
    }
    result.uncovered_elements = detail::collect_elements(covered, false);
    return result;
}

}  // namespace advanced_set_cover_internal

template<class Weight>
[[nodiscard]] PrimalDualSetCoverResult primal_dual_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<Weight>& weights
) {
    const detail::SetSystem system =
        detail::normalize_set_system(universe_size, sets);
    const std::vector<long double> cost =
        advanced_set_cover_internal::checked_weights(
            weights, system.sets.size()
        );
    std::vector<long double> residual = cost;
    std::vector<bool> covered(universe_size, false);
    std::vector<unsigned char> selected(system.sets.size(), 0);
    PrimalDualSetCoverResult result;
    result.element_dual.assign(universe_size, 0.0L);
    for(std::size_t element = 0; element < universe_size; ++element){
        if(covered[element]) continue;
        long double increase = std::numeric_limits<long double>::infinity();
        for(const std::size_t set_index : system.incidence[element]){
            increase = std::min(increase, residual[set_index]);
        }
        if(!std::isfinite(increase)) continue;
        result.element_dual[element] += increase;
        std::size_t tight = system.sets.size();
        for(const std::size_t set_index : system.incidence[element]){
            residual[set_index] = std::max(
                0.0L, residual[set_index] - increase
            );
            if(residual[set_index] == 0.0L
               && (tight == system.sets.size() || set_index < tight)){
                tight = set_index;
            }
        }
        if(tight == system.sets.size()) continue;
        selected[tight] = 1;
        for(const std::size_t covered_element : system.sets[tight]){
            covered[covered_element] = true;
        }
    }
    std::vector<std::size_t> selected_indices;
    for(std::size_t set_index = 0; set_index < selected.size(); ++set_index){
        if(selected[set_index] != 0) selected_indices.push_back(set_index);
    }
    result.cover = advanced_set_cover_internal::reverse_delete(
        system, std::move(selected_indices)
    );
    return result;
}

template<class Weight>
[[nodiscard]] SetCoverResult local_ratio_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<Weight>& weights
) {
    const detail::SetSystem system =
        detail::normalize_set_system(universe_size, sets);
    std::vector<long double> residual =
        advanced_set_cover_internal::checked_weights(
            weights, system.sets.size()
        );
    std::vector<bool> covered(universe_size, false);
    std::vector<std::size_t> selected;
    for(std::size_t element = 0; element < universe_size; ++element){
        if(covered[element]) continue;
        long double minimum = std::numeric_limits<long double>::infinity();
        for(const std::size_t set_index : system.incidence[element]){
            minimum = std::min(minimum, residual[set_index]);
        }
        if(!std::isfinite(minimum)) continue;
        std::size_t chosen = system.sets.size();
        for(const std::size_t set_index : system.incidence[element]){
            residual[set_index] = std::max(
                0.0L, residual[set_index] - minimum
            );
            if(residual[set_index] == 0.0L
               && (chosen == system.sets.size() || set_index < chosen)){
                chosen = set_index;
            }
        }
        if(chosen == system.sets.size()) continue;
        selected.push_back(chosen);
        for(const std::size_t covered_element : system.sets[chosen]){
            covered[covered_element] = true;
        }
    }
    return advanced_set_cover_internal::reverse_delete(
        system, std::move(selected)
    );
}

template<class URBG>
[[nodiscard]] SetCoverResult randomized_rounding_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<long double>& fractional,
    URBG& random,
    std::size_t rounds = 1
) {
    const detail::SetSystem system =
        detail::normalize_set_system(universe_size, sets);
    const auto values =
        advanced_set_cover_internal::checked_fractional_solution(
            universe_size, system, fractional, false
        );
    std::vector<unsigned char> selected(system.sets.size(), 0);
    for(std::size_t round = 0; round < rounds; ++round){
        for(std::size_t set_index = 0; set_index < values.size(); ++set_index){
            if(selected[set_index] != 0) continue;
            std::bernoulli_distribution distribution(
                static_cast<double>(values[set_index])
            );
            selected[set_index] =
                static_cast<unsigned char>(distribution(random));
        }
    }
    return advanced_set_cover_internal::selected_to_result(system, selected);
}

[[nodiscard]] inline SetCoverResult deterministic_rounding_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<long double>& fractional
) {
    const detail::SetSystem system =
        detail::normalize_set_system(universe_size, sets);
    const auto values =
        advanced_set_cover_internal::checked_fractional_solution(
            universe_size, system, fractional, true
        );
    std::size_t maximum_frequency = 1;
    for(const auto& incident : system.incidence){
        maximum_frequency = std::max(maximum_frequency, incident.size());
    }
    const long double threshold =
        1.0L / static_cast<long double>(maximum_frequency);
    std::vector<unsigned char> selected(system.sets.size(), 0);
    for(std::size_t set_index = 0; set_index < values.size(); ++set_index){
        if(values[set_index]
           + 64.0L * std::numeric_limits<long double>::epsilon()
           >= threshold){
            selected[set_index] = 1;
        }
    }
    return advanced_set_cover_internal::selected_to_result(system, selected);
}

[[nodiscard]] inline SetCoverResult iterative_rounding_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<long double>& fractional
) {
    const detail::SetSystem system =
        detail::normalize_set_system(universe_size, sets);
    const auto values =
        advanced_set_cover_internal::checked_fractional_solution(
            universe_size, system, fractional, true
        );
    std::vector<unsigned char> selected(system.sets.size(), 0);
    std::vector<bool> covered(universe_size, false);
    while(true){
        std::size_t element = universe_size;
        for(std::size_t candidate = 0; candidate < universe_size; ++candidate){
            if(!covered[candidate]){
                element = candidate;
                break;
            }
        }
        if(element == universe_size) break;
        std::size_t best = system.sets.size();
        for(const std::size_t set_index : system.incidence[element]){
            if(best == system.sets.size()
               || values[best] < values[set_index]
               || (values[best] == values[set_index] && set_index < best)){
                best = set_index;
            }
        }
        if(best == system.sets.size()) break;
        selected[best] = 1;
        for(const std::size_t covered_element : system.sets[best]){
            covered[covered_element] = true;
        }
    }
    return advanced_set_cover_internal::selected_to_result(system, selected);
}

[[nodiscard]] inline SetCoverResult lp_rounding_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<long double>& fractional
) {
    return deterministic_rounding_set_cover(
        universe_size, sets, fractional
    );
}

template<class URBG>
[[nodiscard]] std::vector<unsigned char> dependent_rounding(
    std::vector<long double> fractional,
    URBG& random
) {
    for(const long double value : fractional){
        if(value < 0.0L || value > 1.0L || !std::isfinite(value)){
            throw std::invalid_argument(
                "dependent-rounding values must be finite and in [0, 1]"
            );
        }
    }
    constexpr long double epsilon = 1.0e-18L;
    while(true){
        std::size_t first = fractional.size();
        std::size_t second = fractional.size();
        for(std::size_t index = 0; index < fractional.size(); ++index){
            if(fractional[index] <= epsilon
               || fractional[index] >= 1.0L - epsilon) continue;
            if(first == fractional.size()) first = index;
            else{
                second = index;
                break;
            }
        }
        if(second == fractional.size()) break;
        const long double increase = std::min(
            1.0L - fractional[first], fractional[second]
        );
        const long double decrease = std::min(
            fractional[first], 1.0L - fractional[second]
        );
        std::bernoulli_distribution choose_increase(
            static_cast<double>(decrease / (increase + decrease))
        );
        if(choose_increase(random)){
            fractional[first] += increase;
            fractional[second] -= increase;
        }else{
            fractional[first] -= decrease;
            fractional[second] += decrease;
        }
    }
    std::vector<unsigned char> result(fractional.size(), 0);
    for(std::size_t index = 0; index < fractional.size(); ++index){
        if(fractional[index] <= epsilon) continue;
        if(fractional[index] >= 1.0L - epsilon){
            result[index] = 1;
        }else{
            std::bernoulli_distribution distribution(
                static_cast<double>(fractional[index])
            );
            result[index] = static_cast<unsigned char>(distribution(random));
        }
    }
    return result;
}

template<class Objective>
[[nodiscard]] std::vector<unsigned char> pipage_rounding(
    std::vector<long double> fractional,
    Objective objective
) {
    for(const long double value : fractional){
        if(value < 0.0L || value > 1.0L || !std::isfinite(value)){
            throw std::invalid_argument(
                "pipage-rounding values must be finite and in [0, 1]"
            );
        }
    }
    constexpr long double epsilon = 1.0e-18L;
    while(true){
        std::size_t first = fractional.size();
        std::size_t second = fractional.size();
        for(std::size_t index = 0; index < fractional.size(); ++index){
            if(fractional[index] <= epsilon
               || fractional[index] >= 1.0L - epsilon) continue;
            if(first == fractional.size()) first = index;
            else{
                second = index;
                break;
            }
        }
        if(first == fractional.size()) break;
        if(second == fractional.size()){
            auto zero = fractional;
            auto one = fractional;
            zero[first] = 0.0L;
            one[first] = 1.0L;
            fractional = std::invoke(objective, zero)
                    < std::invoke(objective, one)
                ? std::move(one) : std::move(zero);
            continue;
        }
        const long double increase = std::min(
            1.0L - fractional[first], fractional[second]
        );
        const long double decrease = std::min(
            fractional[first], 1.0L - fractional[second]
        );
        auto upward = fractional;
        upward[first] += increase;
        upward[second] -= increase;
        auto downward = fractional;
        downward[first] -= decrease;
        downward[second] += decrease;
        fractional = std::invoke(objective, downward)
                < std::invoke(objective, upward)
            ? std::move(upward) : std::move(downward);
    }
    std::vector<unsigned char> result(fractional.size(), 0);
    for(std::size_t index = 0; index < fractional.size(); ++index){
        result[index] =
            static_cast<unsigned char>(fractional[index] >= 0.5L);
    }
    return result;
}

}  // namespace approximate::set_cover

#endif  // CPPLIB_SRC_APPROXIMATE_SET_COVER_ADVANCED_SET_COVER_HPP_INCLUDED
