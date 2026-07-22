#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_CAPACITY_LEVEL_SCHEDULE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_CAPACITY_LEVEL_SCHEDULE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_cost_capacity_level_schedule(
    const std::vector<int>& minimum_required_levels,
    long long increase_cost_per_unit,
    long long level_cost_per_period,
    long long decrease_cost_per_unit
){
    if(increase_cost_per_unit < 0 ||
       level_cost_per_period < 0 ||
       decrease_cost_per_unit < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: costs must be nonnegative "
            "(minimum_cost_capacity_level_schedule)."
        );
    }

    int maximum_level = 0;
    for(const int required_level: minimum_required_levels){
        if(required_level < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: required levels must be nonnegative "
                "(minimum_cost_capacity_level_schedule)."
            );
        }
        maximum_level = std::max(maximum_level, required_level);
    }
    if(minimum_required_levels.empty()) return 0;

    using Wide = __int128_t;
    const Wide infinity = Wide(1) << 126;
    const std::size_t state_count =
        static_cast<std::size_t>(maximum_level) + 1;
    std::vector<Wide> previous(state_count, infinity);
    std::vector<Wide> current(state_count, infinity);
    std::vector<Wide> prefix(state_count, infinity);
    std::vector<Wide> suffix(state_count, infinity);
    previous[0] = 0;

    for(const int required_level: minimum_required_levels){
        Wide best = infinity;
        for(std::size_t index = 0; index < state_count; index++){
            const Wide level = static_cast<Wide>(index);
            if(previous[index] != infinity){
                best = std::min(
                    best,
                    previous[index] -
                        increase_cost_per_unit * level
                );
            }
            prefix[index] = best;
        }

        best = infinity;
        for(std::size_t index = state_count; index-- > 0;){
            const Wide level = static_cast<Wide>(index);
            if(previous[index] != infinity){
                best = std::min(
                    best,
                    previous[index] +
                        decrease_cost_per_unit * level
                );
            }
            suffix[index] = best;
        }

        std::fill(current.begin(), current.end(), infinity);
        for(std::size_t index = static_cast<std::size_t>(required_level);
            index < state_count;
            index++){
            const Wide level = static_cast<Wide>(index);
            const Wide transition_cost = std::min(
                prefix[index] + increase_cost_per_unit * level,
                suffix[index] - decrease_cost_per_unit * level
            );
            current[index] =
                transition_cost + level_cost_per_period * level;
        }
        previous.swap(current);
    }

    const Wide answer = *std::min_element(previous.begin(), previous.end());
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: result overflow "
            "(minimum_cost_capacity_level_schedule)."
        );
    }
    return static_cast<long long>(answer);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_CAPACITY_LEVEL_SCHEDULE_HPP_INCLUDED
