// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/dynamic_li_chao_tree"

#include <cassert>
#include <limits>
#include <random>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp"

int main(){
    DynamicLiChaoTree<-50, 50, 300> cht;
    assert(cht.empty());
    assert(cht.query(0) == decltype(cht)::INF);

    std::vector<std::pair<long long, long long>> lines;
    std::mt19937 rng(246813579);

    for(int step = 0; step < 200; step++){
        long long a = static_cast<int>(rng() % 41) - 20;
        long long b = static_cast<int>(rng() % 201) - 100;
        cht.add_line(a, b);
        lines.push_back({a, b});

        assert(!cht.empty());
        assert(cht.nodes_used() <= static_cast<int>(lines.size()));

        for(long long x = -50; x <= 50; x++){
            long long expected = std::numeric_limits<long long>::max();
            for(auto [aa, bb]: lines){
                expected = std::min(expected, aa * x + bb);
            }
            assert(cht.query(x) == expected);
        }
    }
}
