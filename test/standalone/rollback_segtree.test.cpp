// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/rollback_segtree.hpp"
#include "../../src/structure/types/monoid.hpp"

long long op_sum(long long a, long long b){ return a + b; }
long long e_sum(){ return 0; }
constexpr Monoid<op_sum, e_sum> range_sum;

bool leq_limit(long long x, long long limit){ return x <= limit; }

long long naive_prod(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

int naive_max_right(const std::vector<long long>& a, int l, long long limit){
    long long sum = 0;
    int r = l;
    while(r < static_cast<int>(a.size()) && sum + a[static_cast<std::size_t>(r)] <= limit){
        sum += a[static_cast<std::size_t>(r)];
        r++;
    }
    return r;
}

int naive_min_left(const std::vector<long long>& a, int r, long long limit){
    long long sum = 0;
    int l = r;
    while(l > 0 && sum + a[static_cast<std::size_t>(l - 1)] <= limit){
        l--;
        sum += a[static_cast<std::size_t>(l)];
    }
    return l;
}

void check_all(const RollbackSegtree<range_sum, 128, 512>& seg, const std::vector<long long>& a){
    assert(seg.size() == static_cast<int>(a.size()));
    assert(seg.all_prod() == naive_prod(a, 0, static_cast<int>(a.size())));
    for(int i = 0; i < static_cast<int>(a.size()); i++) assert(seg.get(i) == a[static_cast<std::size_t>(i)]);
    for(int l = 0; l <= static_cast<int>(a.size()); l += 7){
        for(int r = l; r <= static_cast<int>(a.size()); r += 9){
            assert(seg.prod(l, r) == naive_prod(a, l, r));
        }
    }
    for(int l = 0; l <= static_cast<int>(a.size()); l += 5){
        long long limit = naive_prod(a, l, static_cast<int>(a.size())) / 2;
        assert(seg.max_right(l, leq_limit, limit) == naive_max_right(a, l, limit));
    }
    for(int r = 0; r <= static_cast<int>(a.size()); r += 5){
        long long limit = naive_prod(a, 0, r) / 2;
        assert(seg.min_left(r, leq_limit, limit) == naive_min_left(a, r, limit));
    }
}

void self_test(){
    std::vector<long long> a = {1, 2, 3, 4, 5};
    RollbackSegtree<range_sum, 128, 512> seg(a);
    std::vector<std::vector<long long>> states = {a};
    seg.set(2, 10);
    a[2] = 10;
    states.push_back(a);
    int snap = seg.snapshot();
    seg.set(4, 20);
    a[4] = 20;
    states.push_back(a);
    check_all(seg, a);
    seg.rollback(snap);
    states.resize(static_cast<std::size_t>(snap + 1));
    a = states.back();
    check_all(seg, a);
    seg.undo();
    states.pop_back();
    a = states.back();
    check_all(seg, a);
    assert(!seg.can_undo());

    std::mt19937 rng(20261117);
    a.assign(80, 0);
    for(long long& x: a) x = static_cast<int>(rng() % 20);
    RollbackSegtree<range_sum, 128, 512> random_seg(a);
    states = {a};
    for(int step = 0; step < 450; step++){
        int type = static_cast<int>(rng() % 7);
        if(type < 3){
            int k = static_cast<int>(rng() % a.size());
            long long x = static_cast<int>(rng() % 20);
            random_seg.set(k, x);
            a[static_cast<std::size_t>(k)] = x;
            states.push_back(a);
        }else if(type == 3 && states.size() > 1){
            random_seg.undo();
            states.pop_back();
            a = states.back();
        }else if(type == 4){
            int target = static_cast<int>(rng() % states.size());
            random_seg.rollback(target);
            states.resize(static_cast<std::size_t>(target + 1));
            a = states.back();
        }else{
            check_all(random_seg, a);
        }
        assert(random_seg.snapshot() == static_cast<int>(states.size()) - 1);
        assert(random_seg.history_size() == static_cast<int>(states.size()) - 1);
        assert(random_seg.can_undo() == (states.size() > 1));
    }
    check_all(random_seg, a);

    bool thrown = false;
    try{
        random_seg.rollback(random_seg.snapshot() + 1);
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
    RollbackSegtree<range_sum, 256, 2048> seg(a);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int k;
            long long x;
            std::cin >> k >> x;
            seg.set(k, x);
        }else if(type == "GET"){
            int k;
            std::cin >> k;
            std::cout << seg.get(k) << '\n';
        }else if(type == "PROD"){
            int l, r;
            std::cin >> l >> r;
            std::cout << seg.prod(l, r) << '\n';
        }else if(type == "ALL"){
            std::cout << seg.all_prod() << '\n';
        }else if(type == "MR"){
            int l;
            long long limit;
            std::cin >> l >> limit;
            std::cout << seg.max_right(l, leq_limit, limit) << '\n';
        }else if(type == "ML"){
            int r;
            long long limit;
            std::cin >> r >> limit;
            std::cout << seg.min_left(r, leq_limit, limit) << '\n';
        }else if(type == "UNDO"){
            seg.undo();
        }else if(type == "ROLLBACK"){
            int snap;
            std::cin >> snap;
            seg.rollback(snap);
        }else if(type == "SNAP"){
            std::cout << seg.snapshot() << '\n';
        }else if(type == "HISTORY"){
            std::cout << seg.history_size() << '\n';
        }else if(type == "CAN"){
            std::cout << seg.can_undo() << '\n';
        }
    }
}
