#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct FishmongerRouteCost {
    int toll;
    int travel_time;
};

inline FishmongerRouteCost minimum_fishmonger_toll(
    const std::vector<std::vector<int>>& travel_times,
    const std::vector<std::vector<int>>& tolls,
    int time_limit
){
    const int n = static_cast<int>(travel_times.size());
    if(n == 0 || tolls.size() != travel_times.size() || time_limit < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid matrix dimensions "
            "(minimum_fishmonger_toll)."
        );
    }
    for(int row = 0; row < n; row++){
        if(static_cast<int>(travel_times[static_cast<std::size_t>(row)].size()) != n ||
           static_cast<int>(tolls[static_cast<std::size_t>(row)].size()) != n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-square matrix "
                "(minimum_fishmonger_toll)."
            );
        }
    }
    constexpr int infinity = std::numeric_limits<int>::max() / 4;
    struct State { int toll; int used; };
    std::vector<std::vector<State>> dp(
        static_cast<std::size_t>(time_limit + 1),
        std::vector<State>(static_cast<std::size_t>(n), {infinity, infinity})
    );
    for(int remaining = 0; remaining <= time_limit; remaining++){
        dp[static_cast<std::size_t>(remaining)][static_cast<std::size_t>(n - 1)] = {0, 0};
        for(int city = n - 2; city >= 0; city--){
            State best{infinity, infinity};
            for(int next = 0; next < n; next++){
                if(next == city) continue;
                const int duration = travel_times[static_cast<std::size_t>(city)][static_cast<std::size_t>(next)];
                const int toll = tolls[static_cast<std::size_t>(city)][static_cast<std::size_t>(next)];
                if(duration <= 0 || toll < 0)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: invalid edge value "
                        "(minimum_fishmonger_toll)."
                    );
                }
                if(duration > remaining) continue;
                const State suffix = dp[static_cast<std::size_t>(remaining - duration)][static_cast<std::size_t>(next)];
                if(suffix.toll == infinity) continue;
                const State candidate{suffix.toll + toll, suffix.used + duration};
                if(candidate.toll < best.toll ||
                   (candidate.toll == best.toll && candidate.used < best.used)) best = candidate;
            }
            dp[static_cast<std::size_t>(remaining)][static_cast<std::size_t>(city)] = best;
        }
    }
    const State answer = dp.back().front();
    return answer.toll == infinity ? FishmongerRouteCost{-1, -1}
                                   : FishmongerRouteCost{answer.toll, answer.used};
}
