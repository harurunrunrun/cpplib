// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/persistent_dynamic_li_chao_tree"

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/persistent_dynamic_li_chao_tree.hpp"

int main(){
    int input_q;
    if(std::cin >> input_q){
        PersistentDynamicLiChaoTree<-1000, 1000, 2048> cht;
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "ADD"){
                int version;
                long long a, b;
                std::cin >> version >> a >> b;
                std::cout << cht.add_line(version, a, b) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << cht.fork(version) << '\n';
            }else if(type == "QUERY"){
                int version;
                long long x;
                std::cin >> version >> x;
                if(cht.empty(version)) std::cout << "EMPTY\n";
                else std::cout << cht.query(version, x) << '\n';
            }else if(type == "EMPTY"){
                int version;
                std::cin >> version;
                std::cout << cht.empty(version) << '\n';
            }
        }
        return 0;
    }

    PersistentDynamicLiChaoTree<-30, 30, 180> cht;
    std::vector<std::vector<std::pair<long long, long long>>> naive(1);

    assert(cht.versions() == 1);
    assert(cht.latest_version() == 0);
    assert(cht.empty(0));
    assert(cht.query(0, 0) == decltype(cht)::INF);
    bool thrown = false;
    try{
        (void)cht.empty(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)cht.query(0, 31);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    int line_version = cht.add_line(
        0, typename decltype(cht)::Line{2, 3}
    );
    assert(cht.query(line_version, 4) == 11);
    naive.push_back({{2, 3}});

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
    {
        PersistentDynamicLiChaoTree<-2, 2, 4> large;
        int v1 = large.add_line(0, 0, 3000000000000000000LL);
        assert(large.query(v1, 0) == 3000000000000000000LL);
    }
    {
        PersistentDynamicLiChaoTree<1000000, 1000000, 2> saturated;
        int v1 = saturated.add_line(
            0, 1000000000000000LL, 1000000000000000000LL);
        assert(saturated.query(v1, 1000000) ==
               std::numeric_limits<long long>::max());
    }
    {
        PersistentDynamicLiChaoTree<-2, 2, 1> capacity;
        int v1 = capacity.add_line(0, 0, 0);
        int nodes_before = capacity.nodes_used();
        bool thrown = false;
        try{
            (void)capacity.add_line(v1, -1, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        assert(capacity.versions() == 2);
        assert(capacity.nodes_used() == nodes_before);
        for(long long x = -2; x <= 2; x++) assert(capacity.query(v1, x) == 0);
    }
}
