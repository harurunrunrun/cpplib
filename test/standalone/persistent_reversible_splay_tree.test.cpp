// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/bbst/persistent_reversible_splay_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid{};
using Tree = PersistentReversibleSplayTree<add_monoid, 200000, 2000>;

long long sum_naive(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

void check_version(const Tree& tree, int version, const std::vector<long long>& a){
    assert(tree.size(version) == static_cast<int>(a.size()));
    assert(tree.empty(version) == a.empty());
    assert(tree.to_vector(version) == a);
    assert(tree.all_prod(version) == sum_naive(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r, version) == sum_naive(a, l, r));
        }
    }
}

void self_test(){
    auto tree = std::make_unique<Tree>(std::vector<long long>{1, 2, 3});
    std::vector<std::vector<long long>> naive = {{1, 2, 3}};
    int v1 = tree->reverse(0, 3, 0);
    naive.push_back({3, 2, 1});
    int v2 = tree->insert(1, 10, v1);
    naive.push_back({3, 10, 2, 1});
    int v3 = tree->erase(2, v2);
    naive.push_back({3, 10, 1});
    check_version(*tree, 0, naive[0]);
    check_version(*tree, v1, naive[1]);
    check_version(*tree, v2, naive[2]);
    check_version(*tree, v3, naive[3]);

    std::mt19937 rng(20260918);
    for(int step = 0; step < 300; step++){
        int base = static_cast<int>(rng() % naive.size());
        auto next = naive[static_cast<std::size_t>(base)];
        int type = static_cast<int>(rng() % 5);
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
        }else{
            int l = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            if(l > r) std::swap(l, r);
            version = tree->reverse(l, r, base);
            std::reverse(next.begin() + l, next.begin() + r);
        }
        naive.push_back(next);
        assert(version == static_cast<int>(naive.size()) - 1);
        check_version(*tree, base, naive[static_cast<std::size_t>(base)]);
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
        if(type == "REV"){
            std::cout << tree->reverse(l, r, v) << '\n';
        }else if(type == "SUM"){
            std::cout << tree->prod(l, r, v) << '\n';
        }
    }
}