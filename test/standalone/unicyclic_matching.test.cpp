// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/matching/general/unicyclic_matching.hpp"

int brute(int n, const std::vector<UnicyclicMatchingEdge>& edges){
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    int best = 0;
    auto dfs = [&](auto&& self, int id, int count) -> void {
        if(id == static_cast<int>(edges.size())){
            best = std::max(best, count);
            return;
        }
        self(self, id + 1, count);
        const auto& edge = edges[static_cast<std::size_t>(id)];
        if(!used[static_cast<std::size_t>(edge.from)] && !used[static_cast<std::size_t>(edge.to)]){
            used[static_cast<std::size_t>(edge.from)] = used[static_cast<std::size_t>(edge.to)] = 1;
            self(self, id + 1, count + 1);
            used[static_cast<std::size_t>(edge.from)] = used[static_cast<std::size_t>(edge.to)] = 0;
        }
    };
    dfs(dfs, 0, 0);
    return best;
}

bool valid(int n, const std::vector<UnicyclicMatchingEdge>& edges, const UnicyclicMatchingResult& result){
    std::vector<std::vector<char>> exists(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(const auto& edge: edges){
        exists[static_cast<std::size_t>(edge.from)][static_cast<std::size_t>(edge.to)] = 1;
        exists[static_cast<std::size_t>(edge.to)][static_cast<std::size_t>(edge.from)] = 1;
    }
    int count = 0;
    for(int i = 0; i < n; i++){
        int j = result.match[static_cast<std::size_t>(i)];
        if(j == -1) continue;
        if(j < 0 || n <= j) return false;
        if(result.match[static_cast<std::size_t>(j)] != i) return false;
        if(!exists[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)]) return false;
        if(i < j) count++;
    }
    return count == result.size;
}

std::vector<UnicyclicMatchingEdge> make_case(int n, std::mt19937& rng){
    std::vector<UnicyclicMatchingEdge> edges;
    if(n <= 1) return edges;
    int cycle_size = n == 2 ? 2 : 3 + static_cast<int>(rng() % static_cast<unsigned>(n - 2));
    if(cycle_size == 2){
        edges.push_back({0, 1});
        edges.push_back({0, 1});
    }else{
        for(int i = 0; i < cycle_size; i++){
            edges.push_back({i, (i + 1) % cycle_size});
        }
    }
    for(int v = cycle_size; v < n; v++){
        int p = static_cast<int>(rng() % static_cast<unsigned>(v));
        edges.push_back({p, v});
    }
    std::shuffle(edges.begin(), edges.end(), rng);
    return edges;
}

void self_test(){
    {
        bool thrown = false;
        try{
            (void)unicyclic_matching(-1, {});
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        std::vector<UnicyclicMatchingEdge> edges = {
            {0, 1},
            {1, 2},
            {2, 0},
            {3, 4},
        };
        auto res = unicyclic_matching(5, edges);
        assert(valid(5, edges, res));
        assert(res.size == 2);
    }
    std::mt19937 rng(20260906);
    for(int n = 0; n <= 11; n++){
        for(int step = 0; step < 80; step++){
            auto edges = make_case(n, rng);
            auto res = unicyclic_matching(n, edges);
            assert(valid(n, edges, res));
            assert(res.size == brute(n, edges));
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<UnicyclicMatchingEdge> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges){
        std::cin >> edge.from >> edge.to;
    }
    std::cout << unicyclic_matching(n, edges).size << '\n';
}
