// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/range/range_dsu.hpp"

struct NaiveDSU{
    std::vector<int> parent;
    std::vector<int> size;

    explicit NaiveDSU(int n) : parent(static_cast<std::size_t>(n)), size(static_cast<std::size_t>(n), 1){
        std::iota(parent.begin(), parent.end(), 0);
    }

    int leader(int x){
        if(parent[static_cast<std::size_t>(x)] == x) return x;
        return parent[static_cast<std::size_t>(x)] = leader(parent[static_cast<std::size_t>(x)]);
    }

    bool unite(int x, int y){
        x = leader(x);
        y = leader(y);
        if(x == y) return false;
        if(size[static_cast<std::size_t>(x)] < size[static_cast<std::size_t>(y)]) std::swap(x, y);
        parent[static_cast<std::size_t>(y)] = x;
        size[static_cast<std::size_t>(x)] += size[static_cast<std::size_t>(y)];
        return true;
    }

    bool same(int x, int y){
        return leader(x) == leader(y);
    }

    int component_size(int x){
        return size[static_cast<std::size_t>(leader(x))];
    }
};

void self_test(){
    {
        RangeDSU dsu(8);
        dsu.unite_range(1, 4, 1);
        assert(dsu.same(1, 3));
        assert(!dsu.same(0, 1));
        dsu.unite_range(3, 6, 7);
        assert(dsu.same(1, 7));
        assert(dsu.same_range(1, 6));
        assert(dsu.size(7) == 6);
    }
    std::mt19937 rng(20260909);
    for(int step = 0; step < 300; step++){
        int n = 1 + static_cast<int>(rng() % 30);
        RangeDSU dsu(n);
        NaiveDSU naive(n);
        for(int q = 0; q < 200; q++){
            int type = static_cast<int>(rng() % 4);
            if(type == 0){
                int x = static_cast<int>(rng() % static_cast<unsigned>(n));
                int y = static_cast<int>(rng() % static_cast<unsigned>(n));
                dsu.unite(x, y);
                naive.unite(x, y);
            }else if(type == 1){
                int l = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
                int r = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
                if(l > r) std::swap(l, r);
                int x = static_cast<int>(rng() % static_cast<unsigned>(n));
                dsu.unite_range(l, r, x);
                for(int i = l; i < r; i++) naive.unite(i, x);
            }else if(type == 2){
                int x = static_cast<int>(rng() % static_cast<unsigned>(n));
                int y = static_cast<int>(rng() % static_cast<unsigned>(n));
                assert(dsu.same(x, y) == naive.same(x, y));
            }else{
                int x = static_cast<int>(rng() % static_cast<unsigned>(n));
                assert(dsu.size(x) == naive.component_size(x));
            }
            for(int i = 0; i < n; i++){
                for(int j = 0; j < n; j++){
                    assert(dsu.same(i, j) == naive.same(i, j));
                }
            }
        }
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    RangeDSU dsu(n);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "UNITE"){
            int x, y;
            std::cin >> x >> y;
            dsu.unite(x, y);
        }else if(type == "RANGE"){
            int l, r, x;
            std::cin >> l >> r >> x;
            dsu.unite_range(l, r, x);
        }else if(type == "SAME"){
            int x, y;
            std::cin >> x >> y;
            std::cout << dsu.same(x, y) << '\n';
        }else if(type == "SIZE"){
            int x;
            std::cin >> x;
            std::cout << dsu.size(x) << '\n';
        }
    }
}
