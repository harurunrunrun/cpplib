// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/segtree/partially_persistent_dynamic_segtree.hpp"

constexpr long long partial_sum_op(long long a, long long b){ return a + b; }
constexpr long long partial_sum_e(){ return 0; }
constexpr Monoid<partial_sum_op, partial_sum_e> partial_sum_monoid;
using Tree = PartiallyPersistentDynamicSegtree<partial_sum_monoid, 1000000000039LL, 10000, 256, 12000>;

int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S'){
                long long index, value; std::cin >> index >> value; std::cout << tree.set(index, value) << '\n';
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
    assert(tree.versions() == 1 && tree.latest_version() == 0);
    assert(tree.nodes_used() == 0 && tree.changes_used() == 0);
    int v1 = tree.set(999999999999LL, 5);
    int v2 = tree.set(7, 4);
    assert(v1 == 1 && v2 == 2);
    assert(tree.versions() == 3 && tree.latest_version() == v2);
    assert(tree.nodes_used() > 0 && tree.changes_used() > 0);
    assert(tree.all_prod(0) == 0 && tree.all_prod(v1) == 5 && tree.all_prod(v2) == 9);
    assert(tree.get(v1, 7) == 0 && tree.get(v2, 7) == 4);
    assert(tree.max_right(v2, 0, [](long long value){ return value <= 4; }) == 999999999999LL);
    assert(tree.min_left(v2, 1000000000039LL, [](long long value){ return value <= 5; }) == 8);
    PartiallyPersistentDynamicSegtree<partial_sum_monoid, 8, 3, 2, 20> small;
    bool thrown = false;
    try{ (void)small.set(2, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && small.versions() == 1 && small.nodes_used() == 0 && small.changes_used() == 0);
    PartiallyPersistentDynamicSegtree<partial_sum_monoid, 2, 3, 3, 2> history_limited;
    int only_version = history_limited.set(0, 6);
    thrown = false;
    try{ (void)history_limited.set(1, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && history_limited.versions() == 2 && history_limited.latest_version() == only_version);
    assert(history_limited.nodes_used() == 2 && history_limited.changes_used() == 2 && history_limited.all_prod() == 6);
}
