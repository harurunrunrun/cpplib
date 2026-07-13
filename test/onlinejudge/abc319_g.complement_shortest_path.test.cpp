// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc319/tasks/abc319_g

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/complement_shortest_path.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long MOD = 998244353;
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> missing(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int u, v;
        std::cin >> u >> v;
        --u;
        --v;
        missing[static_cast<std::size_t>(u)].push_back(v);
        missing[static_cast<std::size_t>(v)].push_back(u);
    }
    const auto shortest = complement_shortest_path(missing, 0);
    const int target_distance = shortest.dist[static_cast<std::size_t>(n - 1)];
    if(target_distance == -1){
        std::cout << -1 << '\n';
        return 0;
    }

    std::vector<std::vector<int>> level(static_cast<std::size_t>(target_distance + 1));
    for(int v = 0; v < n; ++v){
        const int distance = shortest.dist[static_cast<std::size_t>(v)];
        if(0 <= distance && distance <= target_distance){
            level[static_cast<std::size_t>(distance)].push_back(v);
        }
    }
    std::vector<long long> ways(static_cast<std::size_t>(n), 0);
    ways[0] = 1;
    for(int distance = 1; distance <= target_distance; ++distance){
        long long total = 0;
        for(int v: level[static_cast<std::size_t>(distance - 1)]){
            total += ways[static_cast<std::size_t>(v)];
            if(MOD <= total) total -= MOD;
        }
        for(int v: level[static_cast<std::size_t>(distance)]){
            long long value = total;
            for(int u: missing[static_cast<std::size_t>(v)]){
                if(shortest.dist[static_cast<std::size_t>(u)] == distance - 1){
                    value -= ways[static_cast<std::size_t>(u)];
                    if(value < 0) value += MOD;
                }
            }
            ways[static_cast<std::size_t>(v)] = value;
        }
    }
    std::cout << ways[static_cast<std::size_t>(n - 1)] << '\n';
}
