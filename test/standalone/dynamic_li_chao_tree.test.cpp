// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/dynamic_li_chao_tree"

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp"

int main(){
    int q;
    if(std::cin >> q){
        DynamicLiChaoTree<-1000000, 1000000, 8192> tree;
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "ADD"){
                long long a, b;
                std::cin >> a >> b;
                tree.add_line(a, b);
            }else{
                long long x;
                std::cin >> x;
                std::cout << tree.query(x) << '\n';
            }
        }
        return 0;
    }

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
    {
        DynamicLiChaoTree<-2, 2, 2> large;
        bool thrown = false;
        try{
            (void)large.query(3);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        large.add_line(typename decltype(large)::Line{1, 4});
        assert(large.query(-2) == 2);
    }
    {
        DynamicLiChaoTree<-2, 2, 2> large;
        large.add_line(0, 3000000000000000000LL);
        assert(large.query(0) == 3000000000000000000LL);
    }
    {
        DynamicLiChaoTree<1000000, 1000000, 1> saturated;
        saturated.add_line(1000000000000000LL, 1000000000000000000LL);
        assert(saturated.query(1000000) == std::numeric_limits<long long>::max());
    }
    {
        DynamicLiChaoTree<-2, 2, 1> capacity;
        capacity.add_line(0, 0);
        bool thrown = false;
        try{
            capacity.add_line(-1, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        assert(capacity.nodes_used() == 1);
        for(long long x = -2; x <= 2; x++) assert(capacity.query(x) == 0);
    }
}
