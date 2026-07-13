// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/kth_shortest_walk.hpp"

std::vector<long long> brute_walks(
    const std::vector<std::vector<KthShortestWalkEdge<long long>>>& graph,
    int s,
    int t,
    int k
){
    const int n = static_cast<int>(graph.size());
    std::vector<char> reachable(static_cast<std::size_t>(n), 0);
    std::queue<int> reach_que;
    reachable[static_cast<std::size_t>(s)] = 1;
    reach_que.push(s);
    while(!reach_que.empty()){
        int v = reach_que.front();
        reach_que.pop();
        for(auto e: graph[static_cast<std::size_t>(v)]){
            if(!reachable[static_cast<std::size_t>(e.to)]){
                reachable[static_cast<std::size_t>(e.to)] = 1;
                reach_que.push(e.to);
            }
        }
    }
    if(!reachable[static_cast<std::size_t>(t)]) return {};

    std::vector<long long> result;
    using Pair = std::pair<long long, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    que.push({0, s});
    int guard = 0;
    while(!que.empty() && static_cast<int>(result.size()) < k){
        auto [d, v] = que.top();
        que.pop();
        if(v == t) result.push_back(d);
        for(auto e: graph[static_cast<std::size_t>(v)]){
            que.push({d + e.cost, e.to});
        }
        assert(++guard < 200000);
    }
    return result;
}

void self_test(){
    {
        std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(3);
        graph[0].push_back({1, 1});
        graph[0].push_back({2, 2});
        graph[1].push_back({2, 1});
        graph[1].push_back({1, 2});
        auto res = kth_shortest_walks<long long>(graph, 0, 2, 4);
        assert((res == std::vector<long long>{2, 2, 4, 6}));
        for(int k = 0; k < 4; k++){
            assert(kth_shortest_walk<long long>(graph, 0, 2, k) == res[k]);
            assert(kth_shortest_walk_1indexed<long long>(graph, 0, 2, k + 1) == res[k]);
        }
    }
    {
        std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(2);
        graph[0].push_back({0, 0});
        graph[0].push_back({1, 7});
        graph[0].push_back({1, 7});
        assert(kth_shortest_walk<long long>(graph, 0, 1, 200) == 7);
        assert(kth_shortest_walk_1indexed<long long>(graph, 0, 1, 201) == 7);
        graph[1].push_back({1, 0});
        assert(kth_shortest_walk<long long>(graph, 1, 1, 100) == 0);
    }
    {
        std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(3);
        graph[0].push_back({1, 1});
        assert(!kth_shortest_walk<long long>(graph, 0, 2, 0));
        assert(!kth_shortest_walk_1indexed<long long>(graph, 0, 2, 1));
    }
    {
        std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(2);
        graph[1].push_back({1, -1});
        bool thrown = false;
        try{
            (void)kth_shortest_walk<long long>(graph, 0, 0, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            (void)kth_shortest_walk_1indexed<long long>(graph, 0, 0, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260730);
    for(int n = 1; n <= 9; n++){
        for(int step = 0; step < 100; step++){
            std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u < v && rng() % 4 == 0){
                        graph[static_cast<std::size_t>(u)].push_back({v, 1 + static_cast<int>(rng() % 5)});
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            int t = static_cast<int>(rng() % n);
            int k = 6;
            auto actual = kth_shortest_walks<long long>(graph, s, t, k);
            auto expected = brute_walks(graph, s, t, k);
            assert(actual == expected);
            for(int rank = 0; rank < static_cast<int>(expected.size()); rank++){
                assert(kth_shortest_walk<long long>(graph, s, t, rank) == expected[rank]);
                assert(kth_shortest_walk_1indexed<long long>(graph, s, t, rank + 1) == expected[rank]);
            }
            if(static_cast<int>(expected.size()) < k){
                assert(!kth_shortest_walk<long long>(
                    graph, s, t, static_cast<int>(expected.size())
                ));
            }
        }
    }
}

int main(){
    int n, m, s, t, k;
    if(!(std::cin >> n >> m >> s >> t >> k)){
        self_test();
        return 0;
    }
    std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
    }
    auto res = kth_shortest_walks<long long>(graph, s, t, k);
    std::cout << res.size() << '\n';
    for(std::size_t i = 0; i < res.size(); i++){
        if(i) std::cout << ' ';
        std::cout << res[i];
    }
    std::cout << '\n';
    if(k == 0){
        std::cout << "NONE\n";
    }else{
        auto zero_indexed = kth_shortest_walk<long long>(graph, s, t, k - 1);
        auto one_indexed = kth_shortest_walk_1indexed<long long>(graph, s, t, k);
        assert(zero_indexed == one_indexed);
        if(zero_indexed) std::cout << *zero_indexed << '\n';
        else std::cout << "NONE\n";
    }
}
