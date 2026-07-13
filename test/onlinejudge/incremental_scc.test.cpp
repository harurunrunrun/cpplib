// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/incremental_scc

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/incremental_scc.hpp"
#include "../../src/structure/dsu/dsu.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<FastModint998244353> sum(static_cast<std::size_t>(n));
    for(auto& value: sum) std::cin >> value;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;

    const auto merge_time = incremental_scc(n, edges);
    std::vector<std::vector<int>> events(static_cast<std::size_t>(m + 1));
    for(int edge = 0; edge < m; ++edge){
        const int time = merge_time[static_cast<std::size_t>(edge)];
        if(time <= m) events[static_cast<std::size_t>(time)].push_back(edge);
    }

    DSU<500000> dsu(n);
    FastModint998244353 answer;
    for(int time = 1; time <= m; ++time){
        for(int edge: events[static_cast<std::size_t>(time)]){
            int from = dsu.leader(
                edges[static_cast<std::size_t>(edge)].first
            );
            int to = dsu.leader(
                edges[static_cast<std::size_t>(edge)].second
            );
            if(from == to) continue;
            answer += sum[static_cast<std::size_t>(from)] *
                sum[static_cast<std::size_t>(to)];
            const auto merged_sum = sum[static_cast<std::size_t>(from)] +
                sum[static_cast<std::size_t>(to)];
            dsu.merge(from, to);
            sum[static_cast<std::size_t>(dsu.leader(from))] = merged_sum;
        }
        std::cout << answer << '\n';
    }
}
