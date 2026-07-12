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
#include "../../src/structure/bbst/persistent_lazy_weighted_balanced_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum{};
using Tree = PersistentLazyWeightedBalancedTree<range_add_sum, 400000, 2000>;

constexpr std::int64_t MOD = 1'000'000'007;
constexpr std::int64_t BASE = 911'382'323;

struct Hash{
    std::int64_t value = 0;
    bool operator==(const Hash&) const = default;
};

struct Affine{
    std::int64_t mul = 1;
    std::int64_t add = 0;
    bool operator==(const Affine&) const = default;
};

std::int64_t mod(std::int64_t x){
    x %= MOD;
    return x < 0 ? x + MOD : x;
}

std::int64_t power_base(long long exponent){
    std::int64_t result = 1;
    std::int64_t base = BASE;
    while(exponent > 0){
        if(exponent & 1) result = result * base % MOD;
        base = base * base % MOD;
        exponent >>= 1;
    }
    return result;
}

std::int64_t geometric_sum(long long length){
    std::int64_t result = 0;
    for(long long i = 0; i < length; i++) result = (result * BASE + 1) % MOD;
    return result;
}

Hash hash_op(Hash left, long long, Hash right, long long right_length){
    return {(left.value * power_base(right_length) + right.value) % MOD};
}

Hash hash_e(){
    return {};
}

Hash affine_mapping(Affine f, Hash value, long long length){
    return {(f.mul * value.value + f.add * geometric_sum(length)) % MOD};
}

Affine affine_composition(Affine f, Affine g){
    return {f.mul * g.mul % MOD, (f.mul * g.add + f.add) % MOD};
}

Affine affine_id(){
    return {};
}

constexpr Monoid_Act_Len<
    hash_op,
    hash_e,
    affine_mapping,
    affine_composition,
    affine_id
> affine_hash{};
using HashTree = PersistentLazyWeightedBalancedTree<affine_hash, 350000, 1200>;

long long range_sum(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

Hash range_hash(const std::vector<std::int64_t>& a, int l, int r){
    Hash result{};
    for(int i = l; i < r; i++){
        result = hash_op(result, i - l, {a[static_cast<std::size_t>(i)]}, 1);
    }
    return result;
}

void check_version(const Tree& tree, int version, const std::vector<long long>& a){
    assert(tree.size(version) == static_cast<int>(a.size()));
    assert(tree.empty(version) == a.empty());
    assert(tree.to_vector(version) == a);
    assert(tree.all_prod(version) == range_sum(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r, version) == range_sum(a, l, r));
        }
    }
}

void check_hash_version(const HashTree& tree, int version, const std::vector<std::int64_t>& a){
    assert(tree.size(version) == static_cast<int>(a.size()));
    auto values = tree.to_vector(version);
    assert(values.size() == a.size());
    for(std::size_t i = 0; i < a.size(); i++) assert(values[i] == (Hash{a[i]}));
    assert(tree.all_prod(version) == range_hash(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r, version) == range_hash(a, l, r));
        }
    }
}

void branching_sum_test(){
    auto tree = std::make_unique<Tree>(std::vector<long long>{1, 2, 3, 4});
    std::vector<std::vector<long long>> naive = {{1, 2, 3, 4}};
    int v1 = tree->apply(1, 3, 10, 0);
    naive.push_back({1, 12, 13, 4});
    int v2 = tree->reverse(0, 4, v1);
    naive.push_back({4, 13, 12, 1});
    int v3 = tree->set(2, -5, v2);
    naive.push_back({4, 13, -5, 1});
    int v4 = tree->insert(2, 20, 0);
    naive.push_back({1, 2, 20, 3, 4});
    int v5 = tree->erase(1, v1);
    naive.push_back({1, 13, 4});
    assert(v1 == 1 && v2 == 2 && v3 == 3 && v4 == 4 && v5 == 5);
    for(int version = 0; version < tree->versions(); version++){
        check_version(*tree, version, naive[static_cast<std::size_t>(version)]);
    }

    std::mt19937 rng(20260919);
    for(int step = 0; step < 300; step++){
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
        check_version(*tree, base, naive[static_cast<std::size_t>(base)]);
        check_version(*tree, version, next);
    }
}

void noncommutative_affine_test(){
    Affine f{2, 3};
    Affine g{5, 7};
    assert(affine_composition(f, g) != affine_composition(g, f));

    std::vector<std::int64_t> initial = {1, 2, 3, 4, 5};
    std::vector<Hash> encoded;
    for(auto x: initial) encoded.push_back({x});
    auto tree = std::make_unique<HashTree>(encoded);
    std::vector<std::vector<std::int64_t>> naive = {initial};

    int applied = tree->apply(0, 5, f, 0);
    auto after_apply = initial;
    for(auto& x: after_apply) x = mod(f.mul * x + f.add);
    naive.push_back(after_apply);
    int reversed = tree->reverse(1, 5, applied);
    auto after_reverse = after_apply;
    std::reverse(after_reverse.begin() + 1, after_reverse.end());
    naive.push_back(after_reverse);
    int composed = tree->apply(2, 5, g, reversed);
    auto after_composition = after_reverse;
    for(int i = 2; i < 5; i++){
        after_composition[static_cast<std::size_t>(i)] =
            mod(g.mul * after_composition[static_cast<std::size_t>(i)] + g.add);
    }
    naive.push_back(after_composition);
    int branched = tree->apply(0, 3, g, 0);
    auto branch = initial;
    for(int i = 0; i < 3; i++) branch[static_cast<std::size_t>(i)] = mod(g.mul * branch[static_cast<std::size_t>(i)] + g.add);
    naive.push_back(branch);
    assert(applied == 1 && reversed == 2 && composed == 3 && branched == 4);
    for(int version = 0; version < tree->versions(); version++){
        check_hash_version(*tree, version, naive[static_cast<std::size_t>(version)]);
    }

    std::mt19937 rng(20261002);
    for(int step = 0; step < 220; step++){
        int base = static_cast<int>(rng() % naive.size());
        auto next = naive[static_cast<std::size_t>(base)];
        int type = static_cast<int>(rng() % 5);
        int version;
        if(type == 0 || next.empty()){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            std::int64_t x = rng() % 100;
            version = tree->insert(p, {x}, base);
            next.insert(next.begin() + p, x);
        }else if(type == 1){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            version = tree->erase(p, base);
            next.erase(next.begin() + p);
        }else if(type == 2){
            int p = static_cast<int>(rng() % static_cast<unsigned>(next.size()));
            std::int64_t x = rng() % 100;
            version = tree->set(p, {x}, base);
            next[static_cast<std::size_t>(p)] = x;
        }else if(type == 3){
            int l = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(next.size() + 1));
            if(l > r) std::swap(l, r);
            Affine action{static_cast<std::int64_t>(rng() % 7), static_cast<std::int64_t>(rng() % 11)};
            version = tree->apply(l, r, action, base);
            for(int i = l; i < r; i++){
                next[static_cast<std::size_t>(i)] =
                    mod(action.mul * next[static_cast<std::size_t>(i)] + action.add);
            }
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
    using NodeLimited = PersistentLazyWeightedBalancedTree<range_add_sum, 3, 4>;
    auto node_limited = std::make_unique<NodeLimited>(std::vector<long long>{1, 2});
    bool threw = false;
    try{
        (void)node_limited->apply(0, 1, 10, 0);
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
    assert(node_limited->versions() == 1);
    assert(node_limited->to_vector(0) == std::vector<long long>({1, 2}));
    int applied = node_limited->apply(0, 2, 5, 0);
    assert(applied == 1);
    assert(node_limited->to_vector(0) == std::vector<long long>({1, 2}));
    assert(node_limited->to_vector(applied) == std::vector<long long>({6, 7}));

    using VersionLimited = PersistentLazyWeightedBalancedTree<range_add_sum, 16, 2>;
    auto version_limited = std::make_unique<VersionLimited>(std::vector<long long>{1, 2});
    assert(version_limited->apply(0, 2, 3, 0) == 1);
    threw = false;
    try{
        (void)version_limited->apply(0, 0, 4, 0);
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
    assert(version_limited->versions() == 2);
    assert(version_limited->to_vector(0) == std::vector<long long>({1, 2}));
    assert(version_limited->to_vector(1) == std::vector<long long>({4, 5}));
}

void self_test(){
    branching_sum_test();
    noncommutative_affine_test();
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
        }else if(type == "ADD"){
            int version, l, r;
            long long x;
            std::cin >> version >> l >> r >> x;
            std::cout << tree->apply(l, r, x, version) << '\n';
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
