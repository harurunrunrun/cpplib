// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/bbst/persistent_reversible_splay_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr AddMonoid<long long> add_monoid{};
using Tree = PersistentReversibleSplayTree<add_monoid, 350000, 2000>;

struct Hash{
    std::uint64_t value = 0;
    int length = 0;
    bool operator==(const Hash&) const = default;
};

constexpr std::uint64_t HASH_BASE = 1'000'000'007ULL;

std::uint64_t hash_power(int exponent){
    std::uint64_t result = 1;
    for(int i = 0; i < exponent; i++) result *= HASH_BASE;
    return result;
}

Hash hash_op(Hash left, Hash right){
    return {left.value * hash_power(right.length) + right.value, left.length + right.length};
}

Hash hash_e(){
    return {};
}

constexpr Monoid<hash_op, hash_e> hash_monoid{};
using HashTree = PersistentReversibleSplayTree<hash_monoid, 220000, 800>;

long long sum_naive(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

Hash hash_naive(const std::vector<std::uint64_t>& a, int l, int r){
    Hash result{};
    for(int i = l; i < r; i++) result = hash_op(result, {a[static_cast<std::size_t>(i)], 1});
    return result;
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

void check_hash_version(const HashTree& tree, int version, const std::vector<std::uint64_t>& a){
    assert(tree.size(version) == static_cast<int>(a.size()));
    auto values = tree.to_vector(version);
    assert(values.size() == a.size());
    for(std::size_t i = 0; i < a.size(); i++) assert(values[i] == (Hash{a[i], 1}));
    assert(tree.all_prod(version) == hash_naive(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r, version) == hash_naive(a, l, r));
        }
    }
}

void branching_sum_test(){
    auto tree = std::make_unique<Tree>(std::vector<long long>{1, 2, 3});
    std::vector<std::vector<long long>> naive = {{1, 2, 3}};
    int v1 = tree->reverse(0, 3, 0);
    naive.push_back({3, 2, 1});
    int v2 = tree->insert(1, 10, v1);
    naive.push_back({3, 10, 2, 1});
    int v3 = tree->erase(2, v2);
    naive.push_back({3, 10, 1});
    int v4 = tree->push_front(7, 0);
    naive.push_back({7, 1, 2, 3});
    int v5 = tree->push_back(8, v1);
    naive.push_back({3, 2, 1, 8});
    assert(v1 == 1 && v2 == 2 && v3 == 3 && v4 == 4 && v5 == 5);
    for(int version = 0; version < tree->versions(); version++){
        check_version(*tree, version, naive[static_cast<std::size_t>(version)]);
    }

    std::mt19937 rng(20260918);
    for(int step = 0; step < 320; step++){
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

void noncommutative_test(){
    std::vector<std::uint64_t> initial = {1, 2, 3, 4, 5};
    std::vector<Hash> encoded;
    for(auto x: initial) encoded.push_back({x, 1});
    auto tree = std::make_unique<HashTree>(encoded);
    std::vector<std::vector<std::uint64_t>> naive = {initial};
    std::mt19937 rng(20261001);
    for(int step = 0; step < 180; step++){
        int base = static_cast<int>(rng() % naive.size());
        auto next = naive[static_cast<std::size_t>(base)];
        int type = static_cast<int>(rng() % 4);
        int version;
        if(type == 0 || next.empty()){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            std::uint64_t x = rng() % 100 + 1;
            version = tree->insert(p, {x, 1}, base);
            next.insert(next.begin() + p, x);
        }else if(type == 1){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            std::uint64_t x = rng() % 100 + 1;
            version = tree->set(p, {x, 1}, base);
            next[static_cast<std::size_t>(p)] = x;
        }else if(type == 2){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            version = tree->erase(p, base);
            next.erase(next.begin() + p);
        }else{
            int l = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            if(l > r) std::swap(l, r);
            version = tree->reverse(l, r, base);
            std::reverse(next.begin() + l, next.begin() + r);
        }
        naive.push_back(next);
        check_hash_version(*tree, base, naive[static_cast<std::size_t>(base)]);
        check_hash_version(*tree, version, next);
    }
}

void capacity_rollback_test(){
    using NodeLimited = PersistentReversibleSplayTree<add_monoid, 7, 4>;
    auto node_limited = std::make_unique<NodeLimited>(std::vector<long long>{1, 2, 3});
    bool threw = false;
    try{
        (void)node_limited->insert(1, 9, 0);
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
    assert(node_limited->versions() == 1);
    assert(node_limited->to_vector(0) == std::vector<long long>({1, 2, 3}));
    int reversed = node_limited->reverse(0, 3, 0);
    assert(reversed == 1);
    assert(node_limited->to_vector(0) == std::vector<long long>({1, 2, 3}));
    assert(node_limited->to_vector(reversed) == std::vector<long long>({3, 2, 1}));

    using VersionLimited = PersistentReversibleSplayTree<add_monoid, 16, 2>;
    auto version_limited = std::make_unique<VersionLimited>(std::vector<long long>{1, 2});
    assert(version_limited->reverse(0, 2, 0) == 1);
    threw = false;
    try{
        (void)version_limited->reverse(0, 0, 0);
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
    assert(version_limited->versions() == 2);
    assert(version_limited->to_vector(0) == std::vector<long long>({1, 2}));
    assert(version_limited->to_vector(1) == std::vector<long long>({2, 1}));

    using ConstructorLimited = PersistentReversibleSplayTree<add_monoid, 2, 2>;
    threw = false;
    try{
        auto too_large = std::make_unique<ConstructorLimited>(std::vector<long long>{1, 2, 3});
        (void)too_large;
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
}

void self_test(){
    branching_sum_test();
    noncommutative_test();
    capacity_rollback_test();
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    if(n == -1){
        self_test();
        return 0;
    }
    std::vector<long long> a(static_cast<std::size_t>(n));
    for(auto& x: a) std::cin >> x;
    auto tree = std::make_unique<Tree>(a);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "INS"){
            int version, p;
            long long x;
            std::cin >> version >> p >> x;
            std::cout << tree->insert(p, x, version) << '\n';
        }else if(type == "ERASE"){
            int version, p;
            std::cin >> version >> p;
            std::cout << tree->erase(p, version) << '\n';
        }else if(type == "SET"){
            int version, p;
            long long x;
            std::cin >> version >> p >> x;
            std::cout << tree->set(p, x, version) << '\n';
        }else if(type == "REV"){
            int version, l, r;
            std::cin >> version >> l >> r;
            std::cout << tree->reverse(l, r, version) << '\n';
        }else if(type == "GET"){
            int version, p;
            std::cin >> version >> p;
            std::cout << tree->get(p, version) << '\n';
        }else{
            int version, l, r;
            std::cin >> version >> l >> r;
            std::cout << tree->prod(l, r, version) << '\n';
        }
    }
}
