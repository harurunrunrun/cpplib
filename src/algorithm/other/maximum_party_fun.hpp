#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

struct PartyOption {
    int fee;
    int fun;
};

struct PartyPlanValue {
    int minimum_fee;
    int maximum_fun;
};

inline PartyPlanValue maximum_party_fun(
    int budget,
    const std::vector<PartyOption>& parties
){
    if(budget < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative budget (maximum_party_fun)."
        );
    }
    std::vector<int> dp(static_cast<std::size_t>(budget + 1));
    for(const auto& party: parties){
        if(party.fee < 0 || party.fun < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative party value "
                "(maximum_party_fun)."
            );
        }
        if(party.fee > budget) continue;
        for(int spent = budget; spent >= party.fee; spent--){
            dp[static_cast<std::size_t>(spent)] = std::max(
                dp[static_cast<std::size_t>(spent)],
                dp[static_cast<std::size_t>(spent - party.fee)] + party.fun
            );
        }
    }
    const int best_fun = *std::max_element(dp.begin(), dp.end());
    const int least_fee = static_cast<int>(
        std::find(dp.begin(), dp.end(), best_fun) - dp.begin()
    );
    return {least_fee, best_fun};
}
