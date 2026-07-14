#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_temporary_staff_cost(
    const std::vector<int>& required_workers,
    long long hiring_cost,
    long long monthly_salary,
    long long severance_cost
){
    if(hiring_cost < 0 || monthly_salary < 0 || severance_cost < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_temporary_staff_cost)."
        );
    }
    int maximum_workers = 0;
    for(const int required: required_workers){
        if(required < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (minimum_temporary_staff_cost)."
            );
        }
        maximum_workers = std::max(maximum_workers, required);
    }
    if(required_workers.empty()) return 0;

    using Wide = __int128_t;
    const Wide infinity = Wide(1) << 120;
    const std::size_t state_count = static_cast<std::size_t>(maximum_workers) + 1;
    std::vector<Wide> previous(state_count, infinity);
    std::vector<Wide> current(state_count, infinity);
    std::vector<Wide> prefix(state_count, infinity);
    std::vector<Wide> suffix(state_count, infinity);
    previous[0] = 0;

    for(const int required: required_workers){
        Wide best = infinity;
        for(int workers = 0; workers <= maximum_workers; workers++){
            const std::size_t index = static_cast<std::size_t>(workers);
            if(previous[index] != infinity){
                best = std::min(
                    best,
                    previous[index] - Wide(hiring_cost) * workers
                );
            }
            prefix[index] = best;
        }
        best = infinity;
        for(int workers = maximum_workers; workers >= 0; workers--){
            const std::size_t index = static_cast<std::size_t>(workers);
            if(previous[index] != infinity){
                best = std::min(
                    best,
                    previous[index] + Wide(severance_cost) * workers
                );
            }
            suffix[index] = best;
        }

        std::fill(current.begin(), current.end(), infinity);
        for(int workers = required; workers <= maximum_workers; workers++){
            const std::size_t index = static_cast<std::size_t>(workers);
            const Wide change_cost = std::min(
                prefix[index] + Wide(hiring_cost) * workers,
                suffix[index] - Wide(severance_cost) * workers
            );
            current[index] = change_cost + Wide(monthly_salary) * workers;
        }
        previous.swap(current);
    }

    const Wide answer = *std::min_element(previous.begin(), previous.end());
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: overflow (minimum_temporary_staff_cost)."
        );
    }
    return static_cast<long long>(answer);
}
