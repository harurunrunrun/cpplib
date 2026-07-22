#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STEINER_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STEINER_TREE_HPP_INCLUDED

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
            if(e.to < 0 || n <= e.to || e.cost < T(0))[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (steiner_tree).");
            }
        }
    }
    if(k == 0) return T(0);
    if(k >= std::numeric_limits<int>::digits)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (steiner_tree).");
    }

    const int full = 1 << k;
    std::vector<std::vector<T>> dp(static_cast<std::size_t>(full), std::vector<T>(static_cast<std::size_t>(n), inf));
    std::vector<std::vector<char>> reachable(
        static_cast<std::size_t>(full),
        std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int i = 0; i < k; i++){
        int terminal = terminals[static_cast<std::size_t>(i)];
        dp[static_cast<std::size_t>(1 << i)][static_cast<std::size_t>(terminal)] = T(0);
        reachable[static_cast<std::size_t>(1 << i)][static_cast<std::size_t>(terminal)] = 1;
    }

    using Pair = std::pair<T, int>;
    for(int mask = 1; mask < full; mask++){
        for(int sub = (mask - 1) & mask; sub; sub = (sub - 1) & mask){
            int other = mask ^ sub;
            if(other == 0) continue;
            for(int v = 0; v < n; v++){
                if(!reachable[static_cast<std::size_t>(sub)][static_cast<std::size_t>(v)] ||
                   !reachable[static_cast<std::size_t>(other)][static_cast<std::size_t>(v)]){
                    continue;
                }
                T candidate = dp[static_cast<std::size_t>(sub)][static_cast<std::size_t>(v)] +
                    dp[static_cast<std::size_t>(other)][static_cast<std::size_t>(v)];
                if(!reachable[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] ||
                   candidate < dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)]){
                    dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] = candidate;
                    reachable[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] = 1;
                }
            }
        }

        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
        for(int v = 0; v < n; v++){
            if(reachable[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)]){
                que.push({dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)], v});
            }
        }
        while(!que.empty()){
            auto [d, v] = que.top();
            que.pop();
            if(dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(v)] != d) continue;
            for(const auto& e: graph[static_cast<std::size_t>(v)]){
                T nd = d + e.cost;
                if(!reachable[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)] ||
                   nd < dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)]){
                    dp[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)] = nd;
                    reachable[static_cast<std::size_t>(mask)][static_cast<std::size_t>(e.to)] = 1;
                    que.push({nd, e.to});
                }
            }
        }
    }
    T result = inf;
    bool found = false;
    for(int v = 0; v < n; v++){
        if(reachable[static_cast<std::size_t>(full - 1)][static_cast<std::size_t>(v)] &&
           (!found || dp[static_cast<std::size_t>(full - 1)][static_cast<std::size_t>(v)] < result)){
            result = dp[static_cast<std::size_t>(full - 1)][static_cast<std::size_t>(v)];
            found = true;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STEINER_TREE_HPP_INCLUDED
