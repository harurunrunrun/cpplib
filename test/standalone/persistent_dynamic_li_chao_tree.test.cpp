#define PROBLEM "https://example.com/standalone/persistent_dynamic_li_chao_tree"

#include <cassert>
#include <limits>
#include <random>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/persistent_dynamic_li_chao_tree.hpp"

int main(){
    PersistentDynamicLiChaoTree<-30, 30, 180> cht;
    std::vector<std::vector<std::pair<long long, long long>>> naive(1);

    assert(cht.versions() == 1);
    assert(cht.latest_version() == 0);
    assert(cht.empty(0));
    assert(cht.query(0, 0) == decltype(cht)::INF);

    std::mt19937 rng(1122334455);

    for(int step = 0; step < 140; step++){
        int base = static_cast<int>(rng() % naive.size());
        int type = static_cast<int>(rng() % 5);

        if(type == 0){
            int created = cht.fork(base);
            naive.push_back(naive[base]);
            assert(created == static_cast<int>(naive.size()) - 1);
        }else{
            long long a = static_cast<int>(rng() % 41) - 20;
            long long b = static_cast<int>(rng() % 201) - 100;
            int created = cht.add_line(base, a, b);
            naive.push_back(naive[base]);
            naive.back().push_back({a, b});
            assert(created == static_cast<int>(naive.size()) - 1);
        }

        for(int version = 0; version < static_cast<int>(naive.size()); version++){
            assert(cht.empty(version) == naive[version].empty());
            for(long long x = -30; x <= 30; x++){
                long long expected = decltype(cht)::INF;
                for(auto [a, b]: naive[version]){
                    expected = std::min(expected, a * x + b);
                }
                assert(cht.query(version, x) == expected);
            }
        }
    }
}
