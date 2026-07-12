// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/segtree/rollback_dynamic_lazysegtree.hpp"

constexpr long long rollback_lazy_op(long long a, long long, long long b, long long){ return a + b; }
constexpr long long rollback_lazy_e(){ return 0; }
constexpr long long rollback_lazy_mapping(long long f, long long x, long long len){ return x + f * len; }
constexpr long long rollback_lazy_composition(long long f, long long g){ return f + g; }
constexpr long long rollback_lazy_id(){ return 0; }
constexpr Monoid_Act_Len<rollback_lazy_op, rollback_lazy_e, rollback_lazy_mapping, rollback_lazy_composition, rollback_lazy_id> rollback_lazy_monoid;
using Tree = RollbackDynamicLazySegtree<rollback_lazy_monoid, 1000000000039LL, 15000, 40000>;

int main(){
    int q;
    if(std::cin >> q){
        long long n; std::cin >> n;
        Tree tree(n);
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                long long index, value; std::cin >> index >> value;
                if(type == 'S') tree.set(index, value); else tree.apply(index, value);
            }else if(type == 'R'){
                long long left, right, value; std::cin >> left >> right >> value; tree.apply(left, right, value);
            }else if(type == 'G'){
                long long index; std::cin >> index; std::cout << tree.get(index) << '\n';
            }else if(type == 'P'){
                long long left, right; std::cin >> left >> right; std::cout << tree.prod(left, right) << '\n';
            }else if(type == 'A'){
                std::cout << tree.all_prod() << '\n';
            }else if(type == 'T'){
                std::cout << tree.snapshot() << '\n';
            }else if(type == 'U') tree.undo();
            else{ int snapshot; std::cin >> snapshot; tree.rollback(snapshot); }
        }
        return 0;
    }
    constexpr long long n = 1000000000039LL;
    Tree tree(n);
    tree.apply(0, n, 1);
    int snapshot = tree.snapshot();
    tree.set(999999999999LL, 5);
    tree.apply(10, 20, 3);
    assert(tree.all_prod() == n + 34);
    tree.rollback(snapshot);
    assert(tree.all_prod() == n && tree.get(999999999999LL) == 1);
    tree.undo();
    assert(tree.all_prod() == 0 && tree.nodes_used() == 0);
    RollbackDynamicLazySegtree<rollback_lazy_monoid, 2, 3, 8> limited;
    limited.set(0, 6);
    bool thrown = false;
    try{ limited.set(1, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && limited.history_size() == 1 && limited.all_prod() == 6);
    limited.undo();
    assert(limited.nodes_used() == 0 && limited.changes_used() == 0);
}
