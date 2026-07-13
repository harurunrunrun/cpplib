// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/matching/maximum_weight_general_matching.hpp"

using Edge = MaximumWeightGeneralMatchingEdge<long long>;

long long brute_maximum_weight(int n, const std::vector<Edge>& edges){
    std::vector<std::vector<char>> exists(
        static_cast<std::size_t>(n),
        std::vector<char>(static_cast<std::size_t>(n), 0)
    );
    std::vector<std::vector<long long>> weights(
        static_cast<std::size_t>(n),
        std::vector<long long>(static_cast<std::size_t>(n), 0)
    );
    for(const auto& edge: edges){
        if(edge.from == edge.to) continue;
        const auto from = static_cast<std::size_t>(edge.from);
        const auto to = static_cast<std::size_t>(edge.to);
        if(!exists[from][to] || weights[from][to] < edge.weight){
            exists[from][to] = exists[to][from] = 1;
            weights[from][to] = weights[to][from] = edge.weight;
        }
    }
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    long long answer = 0;
    auto dfs = [&](auto&& self, long long total) -> void {
        int vertex = -1;
        for(int candidate = 0; candidate < n; ++candidate){
            if(!used[static_cast<std::size_t>(candidate)]){
                vertex = candidate;
                break;
            }
        }
        if(vertex == -1){
            answer = std::max(answer, total);
            return;
        }
        used[static_cast<std::size_t>(vertex)] = 1;
        self(self, total);
        for(int other = vertex + 1; other < n; ++other){
            if(used[static_cast<std::size_t>(other)] ||
               !exists[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(other)]) continue;
            used[static_cast<std::size_t>(other)] = 1;
            self(self, total + weights[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(other)]);
            used[static_cast<std::size_t>(other)] = 0;
        }
        used[static_cast<std::size_t>(vertex)] = 0;
    };
    dfs(dfs, 0);
    return answer;
}

bool valid_matching(
    int n,
    const std::vector<Edge>& edges,
    const MaximumWeightGeneralMatchingResult<long long>& result
){
    int size = 0;
    long long weight = 0;
    for(int vertex = 0; vertex < n; ++vertex){
        const int other = result.match[static_cast<std::size_t>(vertex)];
        if(other == -1) continue;
        if(other < 0 || n <= other || result.match[static_cast<std::size_t>(other)] != vertex) return false;
        if(vertex < other){
            ++size;
            bool exists = false;
            long long best = 0;
            for(const auto& edge: edges){
                if((edge.from == vertex && edge.to == other) ||
                   (edge.from == other && edge.to == vertex)){
                    if(!exists || best < edge.weight){
                        exists = true;
                        best = edge.weight;
                    }
                }
            }
            if(!exists) return false;
            weight += best;
        }
    }
    return size == result.size && weight == result.weight;
}

void self_test(){
    {
        const std::vector<Edge> edges = {{0, 1, 100}, {0, 2, 60}, {1, 3, 1}};
        const auto result = maximum_weight_general_matching(4, edges);
        assert(result.size == 1);
        assert(result.weight == 100);
        assert(valid_matching(4, edges, result));
    }
    {
        const std::vector<Edge> edges = {{0, 1, -1}, {1, 2, -2}, {2, 0, -3}};
        const auto result = maximum_weight_general_matching(3, edges);
        assert(result.size == 0);
        assert(result.weight == 0);
    }
    {
        const std::vector<Edge> edges = {{0, 1, 3}, {1, 0, 7}, {2, 3, 5}, {0, 0, 100}};
        const auto result = maximum_weight_general_matching(4, edges);
        assert(result.size == 2);
        assert(result.weight == 12);
        assert(valid_matching(4, edges, result));
    }

    std::mt19937_64 random(20260714);
    for(int n = 0; n <= 12; ++n){
        for(int iteration = 0; iteration < 120; ++iteration){
            std::vector<Edge> edges;
            for(int from = 0; from < n; ++from){
                for(int to = from + 1; to < n; ++to){
                    if(random() % 3 != 0){
                        edges.push_back({from, to, static_cast<long long>(random() % 41) - 20});
                        if(random() % 5 == 0){
                            edges.push_back({to, from, static_cast<long long>(random() % 41) - 20});
                        }
                    }
                }
            }
            const auto result = maximum_weight_general_matching(n, edges);
            assert(valid_matching(n, edges, result));
            assert(result.weight == brute_maximum_weight(n, edges));
        }
    }

    bool thrown = false;
    try{
        (void)maximum_weight_general_matching<long long>(2, {{0, 2, 1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    bool overflowed = false;
    try{
        using SmallEdge = MaximumWeightGeneralMatchingEdge<signed char>;
        (void)maximum_weight_general_matching<signed char>(
            4,
            std::vector<SmallEdge>{{0, 1, 100}, {2, 3, 100}}
        );
    }catch(const std::overflow_error&){
        overflowed = true;
    }
    assert(overflowed);
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; ++index){
        int from, to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from, to, weight});
    }
    const auto result = maximum_weight_general_matching(n, edges);
    assert(valid_matching(n, edges, result));
    std::cout << result.size << ' ' << result.weight << '\n';
}
