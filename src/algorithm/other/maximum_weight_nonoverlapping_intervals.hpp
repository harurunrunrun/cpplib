#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

struct RentalOffer {
    long long start;
    long long duration;
    long long profit;
};

inline long long maximum_rental_profit(std::vector<RentalOffer> offers){
    for(const auto& offer: offers){
        if(offer.duration <= 0 || offer.profit < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid rental offer "
                "(maximum_rental_profit)."
            );
        }
    }
    std::sort(offers.begin(), offers.end(), [](const auto& left, const auto& right){
        const long long left_end = left.start + left.duration;
        const long long right_end = right.start + right.duration;
        if(left_end != right_end) return left_end < right_end;
        return left.start < right.start;
    });
    const int n = static_cast<int>(offers.size());
    std::vector<long long> ends(static_cast<std::size_t>(n));
    std::vector<long long> dp(static_cast<std::size_t>(n + 1));
    for(int i = 0; i < n; i++){
        ends[static_cast<std::size_t>(i)] =
            offers[static_cast<std::size_t>(i)].start +
            offers[static_cast<std::size_t>(i)].duration;
        const int compatible = static_cast<int>(
            std::upper_bound(
                ends.begin(), ends.begin() + i,
                offers[static_cast<std::size_t>(i)].start
            ) - ends.begin()
        );
        dp[static_cast<std::size_t>(i + 1)] = std::max(
            dp[static_cast<std::size_t>(i)],
            dp[static_cast<std::size_t>(compatible)] +
                offers[static_cast<std::size_t>(i)].profit
        );
    }
    return dp.back();
}
