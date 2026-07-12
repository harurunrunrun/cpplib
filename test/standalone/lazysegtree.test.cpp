// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/monoid_act.hpp"

struct SumSize{
    long long sum;
    int size;
    bool operator==(const SumSize& other) const{
        return sum == other.sum && size == other.size;
    }
};

SumSize op_sum_size(SumSize a, SumSize b){ return {a.sum + b.sum, a.size + b.size}; }
SumSize e_sum_size(){ return {0, 0}; }
SumSize mapping_add(long long f, SumSize x){ return {x.sum + f * x.size, x.size}; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act<op_sum_size, e_sum_size, mapping_add, composition_add, id_add> range_add_sum;

bool leq_limit(SumSize x, long long limit){ return x.sum <= limit; }

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

std::vector<SumSize> build_values(const std::vector<long long>& a){
    std::vector<SumSize> result;
    result.reserve(a.size());
    for(long long x: a) result.push_back({x, 1});
    return result;
}

void check_all(LazySegtree<range_add_sum, 128>& seg, const std::vector<long long>& a){
    assert(seg.all_prod().sum == naive_prod(a, 0, static_cast<int>(a.size())));
    for(int i = 0; i < static_cast<int>(a.size()); i++) assert(seg.get(i).sum == a[static_cast<std::size_t>(i)]);
    for(int l = 0; l <= static_cast<int>(a.size()); l += 7){
        for(int r = l; r <= static_cast<int>(a.size()); r += 9){
            assert(seg.prod(l, r).sum == naive_prod(a, l, r));
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
    LazySegtree<range_add_sum, 128> seg(build_values(a));
    seg.apply(1, 4, 10);
    for(int i = 1; i < 4; i++) a[static_cast<std::size_t>(i)] += 10;
    seg.set(2, {-5, 1});
    a[2] = -5;
    check_all(seg, a);

    std::mt19937 rng(20261117);
    a.assign(80, 0);
    for(long long& x: a) x = static_cast<int>(rng() % 20);
    LazySegtree<range_add_sum, 128> random_seg(build_values(a));
    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 8);
        if(type < 2){
            int k = static_cast<int>(rng() % a.size());
            long long x = static_cast<int>(rng() % 20);
            random_seg.set(k, {x, 1});
            a[static_cast<std::size_t>(k)] = x;
        }else if(type < 5){
            int l = static_cast<int>(rng() % (a.size() + 1));
            int r = static_cast<int>(rng() % (a.size() + 1));
            if(l > r) std::swap(l, r);
            long long x = static_cast<int>(rng() % 10);
            random_seg.apply(l, r, x);
            for(int i = l; i < r; i++) a[static_cast<std::size_t>(i)] += x;
        }else if(type == 5){
            int k = static_cast<int>(rng() % a.size());
            long long x = static_cast<int>(rng() % 10);
            random_seg.apply(k, x);
            a[static_cast<std::size_t>(k)] += x;
        }else{
            check_all(random_seg, a);
        }
    }
    check_all(random_seg, a);

    bool thrown = false;
    try{
        random_seg.prod(0, 81);
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
    LazySegtree<range_add_sum, 256> seg(build_values(a));
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int k;
            long long x;
            std::cin >> k >> x;
            seg.set(k, {x, 1});
        }else if(type == "APPLY"){
            int l, r;
            long long x;
            std::cin >> l >> r >> x;
            seg.apply(l, r, x);
        }else if(type == "APPLY1"){
            int k;
            long long x;
            std::cin >> k >> x;
            seg.apply(k, x);
        }else if(type == "GET"){
            int k;
            std::cin >> k;
            std::cout << seg.get(k).sum << '\n';
        }else if(type == "PROD"){
            int l, r;
            std::cin >> l >> r;
            std::cout << seg.prod(l, r).sum << '\n';
        }else if(type == "ALL"){
            std::cout << seg.all_prod().sum << '\n';
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
        }
    }
}
