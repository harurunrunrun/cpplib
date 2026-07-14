#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

struct ScubaCylinder {
    int oxygen;
    int nitrogen;
    int weight;
};

inline int minimum_scuba_cylinder_weight(
    int required_oxygen,
    int required_nitrogen,
    const std::vector<ScubaCylinder>& cylinders
){
    if(required_oxygen < 0 || required_nitrogen < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative gas requirement "
            "(minimum_scuba_cylinder_weight)."
        );
    }
    constexpr int infinity = std::numeric_limits<int>::max() / 4;
    const int width = required_nitrogen + 1;
    std::vector<int> dp(
        static_cast<std::size_t>((required_oxygen + 1) * width), infinity
    );
    dp[0] = 0;
    for(const auto& cylinder: cylinders){
        if(cylinder.oxygen < 0 || cylinder.nitrogen < 0 || cylinder.weight < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative cylinder value "
                "(minimum_scuba_cylinder_weight)."
            );
        }
        auto next = dp;
        for(int oxygen = 0; oxygen <= required_oxygen; oxygen++){
            for(int nitrogen = 0; nitrogen <= required_nitrogen; nitrogen++){
                const int current = dp[static_cast<std::size_t>(oxygen * width + nitrogen)];
                if(current == infinity) continue;
                const int next_oxygen = std::min(required_oxygen, oxygen + cylinder.oxygen);
                const int next_nitrogen = std::min(required_nitrogen, nitrogen + cylinder.nitrogen);
                auto& destination = next[static_cast<std::size_t>(next_oxygen * width + next_nitrogen)];
                destination = std::min(destination, current + cylinder.weight);
            }
        }
        dp.swap(next);
    }
    const int answer = dp.back();
    return answer == infinity ? -1 : answer;
}
