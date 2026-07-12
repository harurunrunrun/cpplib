// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../../src/structure/segtree/persistent_dynamic_segtree.hpp"

constexpr long long sum_op(long long a, long long b){ return a + b; }
constexpr long long sum_e(){ return 0; }
constexpr Monoid<sum_op, sum_e> sum_monoid;
std::string concat_op(std::string left, std::string right){ return left + right; }
std::string concat_e(){ return {}; }
constexpr Monoid<concat_op, concat_e> concat_monoid;
using Tree = PersistentDynamicSegtree<sum_monoid, 1000000000039LL, 10000, 256>;

int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S'){
                int version; long long index, value; std::cin >> version >> index >> value;
                std::cout << tree.set(version, index, value) << '\n';
            }else if(type == 'F'){
                int version; std::cin >> version; std::cout << tree.fork(version) << '\n';
            }else if(type == 'G'){
                int version; long long index; std::cin >> version >> index; std::cout << tree.get(version, index) << '\n';
            }else if(type == 'P'){
                int version; long long left, right; std::cin >> version >> left >> right;
                std::cout << tree.prod(version, left, right) << '\n';
            }else{
                int version; std::cin >> version; std::cout << tree.all_prod(version) << '\n';
            }
        }
        return 0;
    }
    Tree tree;
    int v1 = tree.set(0, 999999999999LL, 5);
    int v2 = tree.set(0, 7, 3);
    assert(tree.all_prod(0) == 0 && tree.all_prod(v1) == 5 && tree.all_prod(v2) == 3);
    assert(tree.get(v1, 7) == 0 && tree.get(v2, 999999999999LL) == 0);
    int v3 = tree.set(v1, 7, 4);
    assert(tree.prod(v3, 0, 1000000000039LL) == 9);
    assert(tree.max_right(v3, 0, [](long long value){ return value <= 4; }) == 999999999999LL);
    assert(tree.min_left(v3, 1000000000039LL, [](long long value){ return value <= 5; }) == 8);
    PersistentDynamicSegtree<concat_monoid, 16, 30, 4> ordered;
    int ordered_v1 = ordered.set(0, 9, std::string("b"));
    int ordered_v2 = ordered.set(ordered_v1, 2, std::string("a"));
    assert(ordered.prod(ordered_v2, 0, 16) == "ab");    PersistentDynamicSegtree<sum_monoid, 8, 3, 2> small;
    bool thrown = false;
    try{ (void)small.set(0, 2, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && small.versions() == 1 && small.nodes_used() == 0 && small.all_prod(0) == 0);
    PersistentDynamicSegtree<sum_monoid, 2, 4, 1> version_limited;
    int only_version = version_limited.set(0, 0, 6);
    thrown = false;
    try{ (void)version_limited.set(only_version, 1, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && version_limited.versions() == 2 && version_limited.all_prod(only_version) == 6);
}
