#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct SteinerTreeEdge{
    int to;
    T cost;
};

template<class T>
T steiner_tree(
    const std::vector<std::vector<SteinerTreeEdge<T>>>& graph,
    const std::vector<int>& terminals,
    T inf = std::numeric_limits<T>::max() / 4
){
    const int n = static_cast<int>(graph.size());
    const int k = static_cast<int>(terminals.size());
    for(int t: terminals){
        if(t < 0 || n <= t)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (steiner_tree).");
        }
    }
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (steiner_tree).");
            }
        }
    }
    if(k == 0) return T(0);

    const int full = 1 << k;
    std::vector<std::vector<T>> dp(static_cast<std::size_t>(full), std::vector<T>(static_cast<std::size_t>(n), inf));
    for(int i = 0; i < k; i++){
        dp[static_cast<std::size_t>(1 << i)][static_cast<std::size_t>(terminals[static_cast<std::size_t>(i)])] = T(0);
    }

    using Pair = std::pair<T, int>;
    for(int mask = 1; mask < full; mask++){
        for(int sub = (mask - 1) & mask; sub; sub = (sub - 1) & mask){
            int other = mask ^ sub;
            if(other == 0) continue;
            for(int v = 0; v < n; v++){
                T a = dp[static_cast<std::size_t>(sub)][static_cast<std::size_t>(v)];
                T b = dp[static_cast<std::size_t>(other)][static_cast<std::size_t>(v)];
                if(a == inf || b == inf) continue;
                dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] =
                    std::min(dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)], a + b);
            }
        }

        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
        for(int v = 0; v < n; v++){
            T d = dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)];
            if(d != inf) que.push({d, v});
        }
        while(!que.empty()){
            auto [d, v] = que.top();
            que.pop();
            if(dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] != d) continue;
            for(const auto& e: graph[static_cast<std::size_t>(v)]){
                T nd = d + e.cost;
                if(nd < dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)]){
                    dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)] = nd;
                    que.push({nd, e.to});
                }
            }
        }
    }
    return *std::min_element(dp[static_cast<std::size_t>(full - 1)].begin(), dp[static_cast<std::size_t>(full - 1)].end());
}
