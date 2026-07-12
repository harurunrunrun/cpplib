// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/bbst/persistent_lazy_weighted_balanced_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum;

using Tree = PersistentLazyWeightedBalancedTree<range_add_sum, 250000, 2000>;

long long range_sum(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

void check_version(const Tree& tree, int version, const std::vector<long long>& a){
    assert(tree.to_vector(version) == a);
    assert(tree.all_prod(version) == range_sum(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r, version) == range_sum(a, l, r));
        }
    }
}

void self_test(){
    auto tree = std::make_unique<Tree>(std::vector<long long>{1, 2, 3, 4});
    std::vector<std::vector<long long>> naive = {{1, 2, 3, 4}};
    int v1 = tree->apply(0, 2, 5, 0);
    naive.push_back({6, 7, 3, 4});
    int v2 = tree->reverse(1, 4, v1);
    naive.push_back({6, 4, 3, 7});
    check_version(*tree, 0, naive[0]);
    check_version(*tree, v1, naive[1]);
    check_version(*tree, v2, naive[2]);

    std::mt19937 rng(20260920);
    for(int step = 0; step < 250; step++){
        int base = static_cast<int>(rng() % naive.size());
        auto next = naive[static_cast<std::size_t>(base)];
        int type = static_cast<int>(rng() % 6);
        int version;
        if(type == 0 || next.empty()){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            long long x = static_cast<int>(rng() % 101) - 50;
            version = tree->insert(p, x, base);
            next.insert(next.begin() + p, x);
        }else if(type == 1){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            version = tree->erase(p, base);
            next.erase(next.begin() + p);
        }else if(type == 2){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            long long x = static_cast<int>(rng() % 101) - 50;
            version = tree->set(p, x, base);
            next[static_cast<std::size_t>(p)] = x;
        }else if(type == 3){
            int l = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            if(l > r) std::swap(l, r);
            long long x = static_cast<int>(rng() % 51) - 25;
            version = tree->apply(l, r, x, base);
            for(int i = l; i < r; i++) next[static_cast<std::size_t>(i)] += x;
        }else{
            int l = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            if(l > r) std::swap(l, r);
            version = tree->reverse(l, r, base);
            std::reverse(next.begin() + l, next.begin() + r);
        }
        naive.push_back(next);
        assert(version == static_cast<int>(naive.size()) - 1);
        check_version(*tree, version, next);
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<long long> a(static_cast<std::size_t>(n));
    for(auto& x: a) std::cin >> x;
    auto tree = std::make_unique<Tree>(a);
    while(q--){
        std::string type;
        int v, l, r;
        std::cin >> type >> v >> l >> r;
        if(type == "ADD"){
            long long x;
            std::cin >> x;
            std::cout << tree->apply(l, r, x, v) << '\n';
        }else if(type == "REV"){
            std::cout << tree->reverse(l, r, v) << '\n';
        }else if(type == "SUM"){
            std::cout << tree->prod(l, r, v) << '\n';
        }
    }
}
