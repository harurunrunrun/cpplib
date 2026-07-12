// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/widesegtree.hpp"

long long naive_prod(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

void check_all(Wide_Segtree<1024>& seg, const std::vector<long long>& a){
    assert(seg.all_prod() == naive_prod(a, 0, static_cast<int>(a.size())));
    for(int i = 0; i < static_cast<int>(a.size()); i++){
        assert(seg.get(i) == a[static_cast<std::size_t>(i)]);
    }
    for(int l = 0; l <= static_cast<int>(a.size()); l += 17){
        for(int r = l; r <= static_cast<int>(a.size()); r += 19){
            assert(seg.prod(l, r) == naive_prod(a, l, r));
        }
    }
}

void self_test(){
    Wide_Segtree<1024> seg;
    std::vector<long long> naive(1024);
    assert(seg.all_prod() == 0);
    seg.set(10, 5);
    naive[10] = 5;
    seg.add(10, 7);
    naive[10] += 7;
    seg.set(1000, -3);
    naive[1000] = -3;
    assert(seg.get(10) == 12);
    assert(seg.prod(0, 11) == 12);
    assert(seg.all_prod() == 9);

    Wide_Segtree<1024> copied = seg;
    assert(copied.prod(0, 1024) == seg.prod(0, 1024));
    copied.add(10, 100);
    assert(seg.get(10) == 12);
    assert(copied.get(10) == 112);
    copied = seg;
    assert(copied.get(10) == 12);

    std::mt19937 rng(20261115);
    for(int step = 0; step < 4000; step++){
        int idx = static_cast<int>(rng() % naive.size());
        int type = static_cast<int>(rng() % 8);
        if(type < 3){
            long long x = static_cast<int>(rng() % 2001) - 1000;
            seg.set(idx, x);
            naive[static_cast<std::size_t>(idx)] = x;
        }else if(type < 6){
            long long x = static_cast<int>(rng() % 201) - 100;
            seg.add(idx, x);
            naive[static_cast<std::size_t>(idx)] += x;
        }else if(type == 6){
            int l = static_cast<int>(rng() % (naive.size() + 1));
            int r = static_cast<int>(rng() % (naive.size() + 1));
            if(l > r) std::swap(l, r);
            assert(seg.prod(l, r) == naive_prod(naive, l, r));
        }else{
            check_all(seg, naive);
        }
    }
    check_all(seg, naive);

    bool thrown = false;
    try{
        seg.get(1024);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    Wide_Segtree<1024> seg;
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int i;
            long long x;
            std::cin >> i >> x;
            seg.set(i, x);
        }else if(type == "ADD"){
            int i;
            long long x;
            std::cin >> i >> x;
            seg.add(i, x);
        }else if(type == "GET"){
            int i;
            std::cin >> i;
            std::cout << seg.get(i) << '\n';
        }else if(type == "PROD"){
            int l, r;
            std::cin >> l >> r;
            std::cout << seg.prod(l, r) << '\n';
        }else if(type == "ALL"){
            std::cout << seg.all_prod() << '\n';
        }
    }
}
