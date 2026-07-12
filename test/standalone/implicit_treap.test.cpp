// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/tree/implicit_treap.hpp"
#include "../../src/structure/types/monoid.hpp"

long long add_ll(long long a, long long b){ return a + b; }
long long zero_ll(){ return 0; }
constexpr Monoid<add_ll, zero_ll> sum_monoid;

std::string concat_string(std::string a, std::string b){ return a + b; }
std::string empty_string(){ return ""; }
constexpr Monoid<concat_string, empty_string> concat_monoid;

long long range_sum(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

void check_all(ImplicitTreap<sum_monoid, 512>& tree, const std::vector<long long>& a){
    assert(tree.size() == static_cast<int>(a.size()));
    assert(tree.empty() == a.empty());
    assert(tree.all_prod() == range_sum(a, 0, static_cast<int>(a.size())));
    for(int l = 0; l <= static_cast<int>(a.size()); l++){
        for(int r = l; r <= static_cast<int>(a.size()); r++){
            assert(tree.prod(l, r) == range_sum(a, l, r));
        }
    }
    for(int i = 0; i < static_cast<int>(a.size()); i++){
        assert(tree.get(i) == a[static_cast<std::size_t>(i)]);
    }
}

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<long long> input(static_cast<std::size_t>(input_n));
        for(long long& value: input) std::cin >> value;
        ImplicitTreap<sum_monoid, 1024> tree(input);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int p;
                long long value;
                std::cin >> p >> value;
                tree.insert(p, value);
            }else if(type == "ERASE"){
                int p;
                std::cin >> p;
                tree.erase(p);
            }else if(type == "SET"){
                int p;
                long long value;
                std::cin >> p >> value;
                tree.set(p, value);
            }else if(type == "REVERSE"){
                int l, r;
                std::cin >> l >> r;
                tree.reverse(l, r);
            }else if(type == "PROD"){
                int l, r;
                std::cin >> l >> r;
                std::cout << tree.prod(l, r) << '\n';
            }else if(type == "GET"){
                int p;
                std::cin >> p;
                std::cout << tree.get(p) << '\n';
            }else if(type == "SIZE"){
                std::cout << tree.size() << '\n';
            }else if(type == "ALL"){
                std::cout << tree.all_prod() << '\n';
            }
        }
        return 0;
    }

    {
        std::vector<std::string> value = {"a", "b", "c", "d"};
        ImplicitTreap<concat_monoid, 16> tree(value);
        assert(tree.prod(0, 4) == "abcd");
        tree.reverse(1, 3);
        assert(tree.prod(0, 4) == "acbd");
        tree.reverse(0, 4);
        assert(tree.prod(0, 4) == "dbca");
        tree.insert(2, "x");
        assert(tree.prod(0, 5) == "dbxca");
        tree.erase(1);
        assert(tree.prod(0, 4) == "dxca");
    }

    std::mt19937 rng(20260713);
    std::vector<long long> init(60);
    for(auto& x: init) x = static_cast<int>(rng() % 200) - 80;
    ImplicitTreap<sum_monoid, 512> tree(init);
    std::vector<long long> naive = init;

    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 10);
        if(type < 2 && static_cast<int>(naive.size()) < 500){
            int p = static_cast<int>(rng() % (naive.size() + 1));
            long long x = static_cast<int>(rng() % 200) - 80;
            tree.insert(p, x);
            naive.insert(naive.begin() + p, x);
        }else if(type < 4 && !naive.empty()){
            int p = static_cast<int>(rng() % naive.size());
            tree.erase(p);
            naive.erase(naive.begin() + p);
        }else if(type < 6 && !naive.empty()){
            int p = static_cast<int>(rng() % naive.size());
            long long x = static_cast<int>(rng() % 200) - 80;
            tree.set(p, x);
            naive[static_cast<std::size_t>(p)] = x;
        }else if(type < 8){
            int l = static_cast<int>(rng() % (naive.size() + 1));
            int r = static_cast<int>(rng() % (naive.size() + 1));
            if(l > r) std::swap(l, r);
            tree.reverse(l, r);
            std::reverse(naive.begin() + l, naive.begin() + r);
        }else{
            int l = static_cast<int>(rng() % (naive.size() + 1));
            int r = static_cast<int>(rng() % (naive.size() + 1));
            if(l > r) std::swap(l, r);
            assert(tree.prod(l, r) == range_sum(naive, l, r));
        }
    }
    check_all(tree, naive);
}
