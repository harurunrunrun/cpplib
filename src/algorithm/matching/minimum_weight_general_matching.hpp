#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct MinimumWeightGeneralMatchingEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct MinimumWeightGeneralMatchingResult{
    int size;
    T cost;
    std::vector<int> match;
};

namespace minimum_weight_general_matching_internal{

template<class T>
struct State{
    int size = -1;
    T cost = T(0);
};

template<class T>
bool better(const State<T>& lhs, const State<T>& rhs){
    if(lhs.size != rhs.size) return lhs.size > rhs.size;
    return lhs.cost < rhs.cost;
}

} // namespace minimum_weight_general_matching_internal

template<class T>
MinimumWeightGeneralMatchingResult<T> minimum_weight_general_matching(
    int n,
    const std::vector<MinimumWeightGeneralMatchingEdge<T>>& edges,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(n < 0 || 22 < n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (minimum_weight_general_matching).");
    }
    std::vector<std::vector<char>> exists(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    std::vector<std::vector<T>> cost(static_cast<std::size_t>(n), std::vector<T>(static_cast<std::size_t>(n), inf));
    for(const auto& e: edges){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (minimum_weight_general_matching).");
        }
        if(e.from == e.to) continue;
        int u = e.from;
        int v = e.to;
        if(!exists[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] ||
           e.cost < cost[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)]){
            exists[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] = 1;
            exists[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)] = 1;
            cost[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] = e.cost;
            cost[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)] = e.cost;
        }
    }

    using State = minimum_weight_general_matching_internal::State<T>;
    const std::uint64_t full = n == 64 ? ~std::uint64_t(0) : ((std::uint64_t(1) << n) - 1);
    std::vector<State> dp(static_cast<std::size_t>(full + 1));
    std::vector<char> seen(static_cast<std::size_t>(full + 1), 0);
    std::vector<int> choice(static_cast<std::size_t>(full + 1), -1);

    auto solve = [&](auto&& self, std::uint64_t mask) -> State {
        if(mask == 0) return {0, T(0)};
        if(seen[static_cast<std::size_t>(mask)]) return dp[static_cast<std::size_t>(mask)];
        seen[static_cast<std::size_t>(mask)] = 1;
        int v = __builtin_ctzll(mask);
        std::uint64_t rest = mask ^ (std::uint64_t(1) << v);
        State best = self(self, rest);
        choice[static_cast<std::size_t>(mask)] = -1;
        for(int u = v + 1; u < n; u++){
            if(!(rest >> u & 1U)) continue;
            if(!exists[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)]) continue;
            State candidate = self(self, rest ^ (std::uint64_t(1) << u));
            candidate.size++;
            candidate.cost += cost[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)];
            if(minimum_weight_general_matching_internal::better(candidate, best)){
                best = candidate;
                choice[static_cast<std::size_t>(mask)] = u;
            }
        }
        dp[static_cast<std::size_t>(mask)] = best;
        return best;
    };

    State best = solve(solve, full);
    MinimumWeightGeneralMatchingResult<T> result;
    result.size = best.size;
    result.cost = best.cost;
    result.match.assign(static_cast<std::size_t>(n), -1);
    std::uint64_t mask = full;
    while(mask){
        int v = __builtin_ctzll(mask);
        int u = choice[static_cast<std::size_t>(mask)];
        mask ^= std::uint64_t(1) << v;
        if(u != -1){
            result.match[static_cast<std::size_t>(v)] = u;
            result.match[static_cast<std::size_t>(u)] = v;
            mask ^= std::uint64_t(1) << u;
        }
    }
    return result;
}
