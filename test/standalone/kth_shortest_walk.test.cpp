// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/kth_shortest_walk.hpp"

using Graph = std::vector<std::vector<KthShortestWalkEdge<long long>>>;

std::vector<long long> brute_walks(
    const Graph& graph,
    int source,
    int target,
    int k
){
    const int n = static_cast<int>(graph.size());
    std::vector<long long> result;
    if(k == 0) return result;

    using State = std::pair<long long, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    std::vector<int> pop_count(static_cast<std::size_t>(n));
    queue.push({0, source});
    while(!queue.empty() && static_cast<int>(result.size()) < k){
        auto [distance, vertex] = queue.top();
        queue.pop();
        int& count = pop_count[static_cast<std::size_t>(vertex)];
        if(count == k) continue;
        ++count;
        if(vertex == target) result.push_back(distance);
        for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
            queue.push({distance + edge.cost, edge.to});
        }
    }
    return result;
}

void check_against_brute(
    const Graph& graph,
    int source,
    int target,
    int k
){
    const auto expected = brute_walks(graph, source, target, k);
    const auto actual = kth_shortest_walks<long long>(
        graph, source, target, k
    );
    assert(actual == expected);
    for(int rank = 0; rank < static_cast<int>(expected.size()); rank++){
        assert(kth_shortest_walk<long long>(
            graph, source, target, rank
        ) == expected[static_cast<std::size_t>(rank)]);
        assert(kth_shortest_walk_1indexed<long long>(
            graph, source, target, rank + 1
        ) == expected[static_cast<std::size_t>(rank)]);
    }
    if(static_cast<int>(expected.size()) < k){
        const int missing = static_cast<int>(expected.size());
        assert(!kth_shortest_walk<long long>(
            graph, source, target, missing
        ));
        assert(!kth_shortest_walk_1indexed<long long>(
            graph, source, target, missing + 1
        ));
    }
}

void self_test(){
    {
        Graph graph(3);
        graph[0].push_back({1, 1});
        graph[0].push_back({2, 2});
        graph[1].push_back({2, 1});
        graph[1].push_back({1, 2});
        auto result = kth_shortest_walks<long long>(graph, 0, 2, 4);
        assert((result == std::vector<long long>{2, 2, 4, 6}));
        check_against_brute(graph, 0, 2, 20);
    }
    {
        // A zero-cost cycle and parallel edges produce infinitely many walks
        // with the same distance.
        Graph graph(2);
        graph[0].push_back({0, 0});
        graph[0].push_back({1, 7});
        graph[0].push_back({1, 7});
        check_against_brute(graph, 0, 1, 250);
        assert(kth_shortest_walk<long long>(graph, 0, 1, 200) == 7);
        graph[1].push_back({1, 0});
        check_against_brute(graph, 1, 1, 150);
    }
    {
        // The zero-cost cycle is away from both endpoints and contains
        // alternative exits with equal cost.
        Graph graph(4);
        graph[0].push_back({1, 2});
        graph[1].push_back({2, 0});
        graph[2].push_back({1, 0});
        graph[1].push_back({3, 3});
        graph[2].push_back({3, 3});
        graph[2].push_back({3, 3});
        check_against_brute(graph, 0, 3, 200);
    }
    {
        // The empty walk is first. Positive cycles and parallel self-loops
        // still have to be counted as different walks.
        Graph graph(2);
        graph[0].push_back({1, 2});
        graph[1].push_back({0, 3});
        graph[0].push_back({0, 7});
        graph[0].push_back({0, 7});
        check_against_brute(graph, 0, 0, 100);
        assert(kth_shortest_walk<long long>(graph, 0, 0, 0) == 0);
    }
    {
        // A DAG has only finitely many walks. Parallel edges are distinct.
        Graph graph(5);
        graph[0].push_back({1, 1});
        graph[0].push_back({1, 1});
        graph[0].push_back({2, 2});
        graph[1].push_back({3, 2});
        graph[1].push_back({4, 8});
        graph[2].push_back({3, 1});
        graph[3].push_back({4, 3});
        check_against_brute(graph, 0, 4, 20);
    }
    {
        Graph graph(3);
        graph[0].push_back({1, 1});
        check_against_brute(graph, 0, 2, 6);
    }
    {
        Graph graph(2);
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

        thrown = false;
        try{
            (void)kth_shortest_walks<long long>(graph, 0, 0, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    // Directed cyclic random graphs include zero edges, zero cycles, self
    // loops, and parallel edges. The bounded best-first search is the oracle.
    std::mt19937 rng(20260730);
    for(int n = 1; n <= 8; n++){
        for(int step = 0; step < 200; step++){
            Graph graph(static_cast<std::size_t>(n));
            for(int from = 0; from < n; from++){
                for(int to = 0; to < n; to++){
                    if(rng() % 5 == 0){
                        const long long cost = static_cast<long long>(rng() % 6);
                        graph[static_cast<std::size_t>(from)].push_back({to, cost});
                        if(rng() % 5 == 0){
                            graph[static_cast<std::size_t>(from)].push_back(
                                {to, static_cast<long long>(rng() % 2 == 0 ? cost : rng() % 6)}
                            );
                        }
                    }
                }
            }
            const int source = static_cast<int>(rng() % static_cast<unsigned>(n));
            const int target = static_cast<int>(rng() % static_cast<unsigned>(n));
            check_against_brute(graph, source, target, 20);
        }
    }

    // This distinguishes sidetrack enumeration from expanding every edge for
    // every rank: the old O(KN) search needs about 1.2 billion expansions.
    {
        constexpr int n = 40000;
        constexpr int k = 30000;
        Graph graph(static_cast<std::size_t>(n));
        graph[0].push_back({0, 0});
        for(int vertex = 0; vertex + 1 < n; vertex++){
            graph[static_cast<std::size_t>(vertex)].push_back({vertex + 1, 1});
        }
        const auto result = kth_shortest_walks<long long>(
            graph, 0, n - 1, k
        );
        assert(result.size() == static_cast<std::size_t>(k));
        assert(result.front() == n - 1);
        assert(result.back() == n - 1);
        assert(kth_shortest_walk<long long>(graph, 0, n - 1, k - 1) == n - 1);
        assert(kth_shortest_walk_1indexed<long long>(
            graph, 0, n - 1, k
        ) == n - 1);
    }
}

int main(){
    int n, m, source, target, k;
    if(!(std::cin >> n >> m >> source >> target >> k)){
        self_test();
        return 0;
    }
    Graph graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    const auto result = kth_shortest_walks<long long>(
        graph, source, target, k
    );
    std::cout << result.size() << '\n';
    for(std::size_t index = 0; index < result.size(); index++){
        if(index != 0) std::cout << ' ';
        std::cout << result[index];
    }
    std::cout << '\n';
    if(k == 0){
        std::cout << "NONE\n";
    }else{
        const auto zero_indexed = kth_shortest_walk<long long>(
            graph, source, target, k - 1
        );
        const auto one_indexed = kth_shortest_walk_1indexed<long long>(
            graph, source, target, k
        );
        assert(zero_indexed == one_indexed);
        if(zero_indexed) std::cout << *zero_indexed << '\n';
        else std::cout << "NONE\n";
    }
}
