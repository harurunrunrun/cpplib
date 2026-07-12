// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/dualsegtree.hpp"
#include "../../src/structure/types/monoid_act.hpp"

long long op_dummy(long long, long long){ return 0; }
long long e_zero(){ return 0; }
long long mapping_add(long long f, long long x){ return x + f; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_zero(){ return 0; }
constexpr Monoid_Act<op_dummy, e_zero, mapping_add, composition_add, id_zero> range_add_point_get;

void check_all(DualSegtree<range_add_point_get, 128>& seg, const std::vector<long long>& a){
    for(int i = 0; i < static_cast<int>(a.size()); i++) assert(seg.get(i) == a[static_cast<std::size_t>(i)]);
}

void self_test(){
    std::vector<long long> a = {1, 2, 3, 4, 5};
    DualSegtree<range_add_point_get, 128> seg(a);
    seg.apply(1, 4, 10);
    for(int i = 1; i < 4; i++) a[static_cast<std::size_t>(i)] += 10;
    seg.set(2, -5);
    a[2] = -5;
    check_all(seg, a);

    std::mt19937 rng(20261117);
    a.assign(80, 0);
    for(long long& x: a) x = static_cast<int>(rng() % 100) - 50;
    DualSegtree<range_add_point_get, 128> random_seg(a);
    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 6);
        if(type < 2){
            int k = static_cast<int>(rng() % a.size());
            long long x = static_cast<int>(rng() % 200) - 100;
            random_seg.set(k, x);
            a[static_cast<std::size_t>(k)] = x;
        }else if(type < 5){
            int l = static_cast<int>(rng() % (a.size() + 1));
            int r = static_cast<int>(rng() % (a.size() + 1));
            if(l > r) std::swap(l, r);
            long long x = static_cast<int>(rng() % 101) - 50;
            random_seg.apply(l, r, x);
            for(int i = l; i < r; i++) a[static_cast<std::size_t>(i)] += x;
        }else{
            check_all(random_seg, a);
        }
    }
    check_all(random_seg, a);

    bool thrown = false;
    try{
        random_seg.apply(-1, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<long long> a(static_cast<std::size_t>(n));
    for(auto& x: a) std::cin >> x;
    DualSegtree<range_add_point_get, 256> seg(a);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int k;
            long long x;
            std::cin >> k >> x;
            seg.set(k, x);
        }else if(type == "APPLY"){
            int l, r;
            long long x;
            std::cin >> l >> r >> x;
            seg.apply(l, r, x);
        }else if(type == "GET"){
            int k;
            std::cin >> k;
            std::cout << seg.get(k) << '\n';
        }
    }
}
