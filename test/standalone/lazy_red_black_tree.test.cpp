// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/bbst/lazy_red_black_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum;

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

void check_all(LazyRedBlackTree<int, range_add_sum, 512>& tree, const std::map<int, long long>& mp){
    assert(tree.size() == static_cast<int>(mp.size()));
    assert(tree.empty() == mp.empty());
    assert(tree.all_prod() == naive_prod(mp, -1000, 1000));
    for(int x = -10; x <= 210; x++){
        assert(tree.contains(x) == (mp.find(x) != mp.end()));
        assert(tree.order_of_key(x) == static_cast<int>(std::distance(mp.begin(), mp.lower_bound(x))));
        auto got = tree.get(x);
        auto it = mp.find(x);
        if(it == mp.end()) assert(!got.has_value());
        else assert(got == it->second);
    }
    for(int l = -5; l <= 205; l += 7){
        for(int r = l; r <= 210; r += 11){
            assert(tree.prod(l, r) == naive_prod(mp, l, r));
        }
    }
    for(int k = -2; k <= static_cast<int>(mp.size()) + 2; k++){
        assert(tree.kth_key(k) == kth_key_naive(mp, k));
    }
}

int main(){
    int input_q;
    if(std::cin >> input_q){
        LazyRedBlackTree<int, range_add_sum, 512> tree;
        auto print_optional = [](const std::optional<long long>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        auto print_optional_key = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int key;
                long long value;
                std::cin >> key >> value;
                std::cout << tree.insert(key, value) << '\n';
            }else if(type == "ERASE"){
                int key;
                std::cin >> key;
                std::cout << tree.erase(key) << '\n';
            }else if(type == "SET"){
                int key;
                long long value;
                std::cin >> key >> value;
                std::cout << tree.set(key, value) << '\n';
            }else if(type == "APPLY"){
                int l, r;
                long long value;
                std::cin >> l >> r >> value;
                tree.apply(l, r, value);
            }else if(type == "GET"){
                int key;
                std::cin >> key;
                print_optional(tree.get(key));
            }else if(type == "CONTAINS"){
                int key;
                std::cin >> key;
                std::cout << tree.contains(key) << '\n';
            }else if(type == "PROD"){
                int l, r;
                std::cin >> l >> r;
                std::cout << tree.prod(l, r) << '\n';
            }else if(type == "KTH"){
                int k;
                std::cin >> k;
                print_optional_key(tree.kth_key(k));
            }else if(type == "ORDER"){
                int key;
                std::cin >> key;
                std::cout << tree.order_of_key(key) << '\n';
            }else if(type == "SIZE"){
                std::cout << tree.size() << '\n';
            }else if(type == "ALL"){
                std::cout << tree.all_prod() << '\n';
            }
        }
        return 0;
    }

    LazyRedBlackTree<int, range_add_sum, 512> tree;
    std::map<int, long long> naive;
    std::mt19937 rng(42424242);

    for(int step = 0; step < 5000; step++){
        int key = static_cast<int>(rng() % 201);
        int type = static_cast<int>(rng() % 12);
        if(type < 3){
            long long x = static_cast<int>(rng() % 200) - 80;
            bool expected = naive.emplace(key, x).second;
            assert(tree.insert(key, x) == expected);
        }else if(type < 5){
            bool expected = naive.erase(key) != 0;
            assert(tree.erase(key) == expected);
        }else if(type < 7){
            long long x = static_cast<int>(rng() % 200) - 80;
            bool expected = naive.find(key) != naive.end();
            if(expected) naive[key] = x;
            assert(tree.set(key, x) == expected);
        }else if(type < 9){
            int l = static_cast<int>(rng() % 210) - 5;
            int r = static_cast<int>(rng() % 210) - 5;
            if(l > r) std::swap(l, r);
            long long x = static_cast<int>(rng() % 101) - 50;
            tree.apply(l, r, x);
            for(auto it = naive.lower_bound(l); it != naive.end() && it->first < r; ++it){
                it->second += x;
            }
        }else if(type < 11){
            int l = static_cast<int>(rng() % 210) - 5;
            int r = static_cast<int>(rng() % 210) - 5;
            if(l > r) std::swap(l, r);
            assert(tree.prod(l, r) == naive_prod(naive, l, r));
        }else{
            check_all(tree, naive);
        }
    }
    check_all(tree, naive);
}
