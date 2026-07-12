// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/bbst/persistent_lazy_red_black_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum;

using Tree = PersistentLazyRedBlackTree<int, range_add_sum, 300000, 3000>;

long long naive_prod(const std::map<int, long long>& mp, int l, int r){
    long long result = 0;
    for(auto it = mp.lower_bound(l); it != mp.end() && it->first < r; ++it){
        result += it->second;
    }
    return result;
}

std::optional<int> kth_key_naive(const std::map<int, long long>& mp, int k){
    if(k < 0 || static_cast<int>(mp.size()) <= k) return std::nullopt;
    auto it = mp.begin();
    std::advance(it, k);
    return it->first;
}

void check_version(const Tree& tree, int version, const std::map<int, long long>& mp){
    assert(tree.size(version) == static_cast<int>(mp.size()));
    assert(tree.empty(version) == mp.empty());
    assert(tree.all_prod(version) == naive_prod(mp, -1000, 1000));
    for(int x = -10; x <= 70; x++){
        assert(tree.contains(x, version) == (mp.find(x) != mp.end()));
        assert(tree.order_of_key(x, version) == static_cast<int>(std::distance(mp.begin(), mp.lower_bound(x))));
        auto got = tree.get(x, version);
        auto it = mp.find(x);
        if(it == mp.end()){
            assert(!got.has_value());
        }else{
            assert(got == it->second);
        }
    }
    for(int l = -8; l <= 65; l += 5){
        for(int r = l; r <= 75; r += 7){
            assert(tree.prod(l, r, version) == naive_prod(mp, l, r));
        }
    }
    for(int k = -2; k <= static_cast<int>(mp.size()) + 2; k++){
        assert(tree.kth_key(k, version) == kth_key_naive(mp, k));
    }
    std::vector<std::pair<int, long long>> values(mp.begin(), mp.end());
    assert(tree.to_vector(version) == values);
}

void self_test(){
    auto tree = std::make_unique<Tree>();
    std::vector<std::map<int, long long>> naive(1);

    int v1 = tree->insert(5, 10, 0);
    naive.push_back({{5, 10}});
    int v2 = tree->insert(2, 3, v1);
    naive.push_back({{2, 3}, {5, 10}});
    int v3 = tree->apply(0, 6, 2, v2);
    naive.push_back({{2, 5}, {5, 12}});
    int v4 = tree->set(5, 100, v3);
    naive.push_back({{2, 5}, {5, 100}});
    int v5 = tree->erase(2, v4);
    naive.push_back({{5, 100}});
    int v6 = tree->all_apply(-7, v5);
    naive.push_back({{5, 93}});

    assert(v1 == 1);
    assert(v2 == 2);
    assert(v3 == 3);
    assert(v4 == 4);
    assert(v5 == 5);
    assert(v6 == 6);
    for(int v = 0; v < static_cast<int>(naive.size()); v++){
        check_version(*tree, v, naive[static_cast<std::size_t>(v)]);
    }

    std::mt19937 rng(20261113);
    for(int step = 0; step < 700; step++){
        int base = static_cast<int>(rng() % naive.size());
        int key = static_cast<int>(rng() % 61);
        int type = static_cast<int>(rng() % 11);
        if(type < 2){
            long long value = static_cast<int>(rng() % 101) - 50;
            std::map<int, long long> next = naive[static_cast<std::size_t>(base)];
            next.emplace(key, value);
            int version = tree->insert(key, value, base);
            naive.push_back(next);
            assert(version == static_cast<int>(naive.size()) - 1);
            check_version(*tree, base, naive[static_cast<std::size_t>(base)]);
            check_version(*tree, version, next);
        }else if(type < 4){
            std::map<int, long long> next = naive[static_cast<std::size_t>(base)];
            next.erase(key);
            int version = tree->erase(key, base);
            naive.push_back(next);
            assert(version == static_cast<int>(naive.size()) - 1);
            check_version(*tree, version, next);
        }else if(type < 6){
            long long value = static_cast<int>(rng() % 101) - 50;
            std::map<int, long long> next = naive[static_cast<std::size_t>(base)];
            auto it = next.find(key);
            if(it != next.end()) it->second = value;
            int version = tree->set(key, value, base);
            naive.push_back(next);
            assert(version == static_cast<int>(naive.size()) - 1);
            check_version(*tree, version, next);
        }else if(type < 8){
            int l = static_cast<int>(rng() % 70) - 5;
            int r = static_cast<int>(rng() % 70) - 5;
            if(l > r) std::swap(l, r);
            long long add = static_cast<int>(rng() % 41) - 20;
            std::map<int, long long> next = naive[static_cast<std::size_t>(base)];
            for(auto it = next.lower_bound(l); it != next.end() && it->first < r; ++it){
                it->second += add;
            }
            int version = tree->apply(l, r, add, base);
            naive.push_back(next);
            assert(version == static_cast<int>(naive.size()) - 1);
            check_version(*tree, version, next);
        }else if(type == 8){
            long long add = static_cast<int>(rng() % 41) - 20;
            std::map<int, long long> next = naive[static_cast<std::size_t>(base)];
            for(auto& [_, value] : next) value += add;
            int version = tree->all_apply(add, base);
            naive.push_back(next);
            assert(version == static_cast<int>(naive.size()) - 1);
            check_version(*tree, version, next);
        }else{
            check_version(*tree, base, naive[static_cast<std::size_t>(base)]);
        }
    }
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }

    auto tree = std::make_unique<Tree>();
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "INSERT"){
            int v, key;
            long long value;
            std::cin >> v >> key >> value;
            std::cout << tree->insert(key, value, v) << '\n';
        }else if(type == "ERASE"){
            int v, key;
            std::cin >> v >> key;
            std::cout << tree->erase(key, v) << '\n';
        }else if(type == "SET"){
            int v, key;
            long long value;
            std::cin >> v >> key >> value;
            std::cout << tree->set(key, value, v) << '\n';
        }else if(type == "APPLY"){
            int v, l, r;
            long long add;
            std::cin >> v >> l >> r >> add;
            std::cout << tree->apply(l, r, add, v) << '\n';
        }else if(type == "ALL_APPLY"){
            int v;
            long long add;
            std::cin >> v >> add;
            std::cout << tree->all_apply(add, v) << '\n';
        }else if(type == "PROD"){
            int v, l, r;
            std::cin >> v >> l >> r;
            std::cout << tree->prod(l, r, v) << '\n';
        }else if(type == "GET"){
            int v, key;
            std::cin >> v >> key;
            auto value = tree->get(key, v);
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        }else if(type == "SIZE"){
            int v;
            std::cin >> v;
            std::cout << tree->size(v) << '\n';
        }else if(type == "ALL_PROD"){
            int v;
            std::cin >> v;
            std::cout << tree->all_prod(v) << '\n';
        }else if(type == "CONTAINS"){
            int v, key;
            std::cin >> v >> key;
            std::cout << tree->contains(key, v) << '\n';
        }else if(type == "KTH"){
            int v, k;
            std::cin >> v >> k;
            auto key = tree->kth_key(k, v);
            if(key) std::cout << *key << '\n';
            else std::cout << "NONE\n";
        }else if(type == "ORDER"){
            int v, key;
            std::cin >> v >> key;
            std::cout << tree->order_of_key(key, v) << '\n';
        }
    }
}
