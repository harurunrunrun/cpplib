// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/bbst/lazy_reversible_splay_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum;

std::string op_concat(std::string a, long long, std::string b, long long){ return a + b; }
std::string e_concat(){ return ""; }
std::string mapping_id(bool, std::string x, long long){ return x; }
bool composition_id(bool, bool){ return false; }
bool id_bool(){ return false; }
constexpr Monoid_Act_Len<op_concat, e_concat, mapping_id, composition_id, id_bool> concat_act;

long long range_sum(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

void check_all(LazyReversibleSplayTree<range_add_sum, 512>& tree, const std::vector<long long>& a){
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
        LazyReversibleSplayTree<range_add_sum, 1024> tree(input);
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
            }else if(type == "APPLY"){
                int l, r;
                long long value;
                std::cin >> l >> r >> value;
                tree.apply(l, r, value);
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
        LazyReversibleSplayTree<concat_act, 16> tree(value);
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

    std::mt19937 rng(27182818);
    std::vector<long long> init(60);
    for(auto& x: init) x = static_cast<int>(rng() % 200) - 80;
    LazyReversibleSplayTree<range_add_sum, 512> tree(init);
    std::vector<long long> naive = init;

    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 12);
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
            long long x = static_cast<int>(rng() % 101) - 50;
            tree.apply(l, r, x);
            for(int i = l; i < r; i++) naive[static_cast<std::size_t>(i)] += x;
        }else if(type < 10){
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
