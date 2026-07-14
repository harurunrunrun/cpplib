// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/segtree/rollback_dynamic_segtree.hpp"

constexpr long long rollback_sum_op(long long a, long long b){ return a + b; }
constexpr long long rollback_sum_e(){ return 0; }
constexpr Monoid<rollback_sum_op, rollback_sum_e> rollback_sum_monoid;
using Tree = RollbackDynamicSegtree<rollback_sum_monoid, 1000000000039LL, 10000, 12000>;

int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S'){
                long long index, value; std::cin >> index >> value; tree.set(index, value);
            }else if(type == 'G'){
                long long index; std::cin >> index; std::cout << tree.get(index) << '\n';
            }else if(type == 'P'){
                long long left, right; std::cin >> left >> right; std::cout << tree.prod(left, right) << '\n';
            }else if(type == 'A'){
                std::cout << tree.all_prod() << '\n';
            }else if(type == 'T'){
                std::cout << tree.snapshot() << '\n';
            }else if(type == 'U'){
                tree.undo();
            }else{
                int snapshot; std::cin >> snapshot; tree.rollback(snapshot);
            }
        }
        return 0;
    }
    Tree tree;
    assert(tree.history_size() == 0 && tree.snapshot() == 0);
    assert(!tree.can_undo() && tree.nodes_used() == 0 && tree.changes_used() == 0);
    tree.set(999999999999LL, 5);
    int snapshot = tree.snapshot();
    assert(snapshot == 1 && tree.history_size() == 1 && tree.can_undo());
    tree.set(7, 4);
    assert(tree.history_size() == 2 && tree.snapshot() == 2);
    assert(tree.all_prod() == 9);
    tree.rollback(snapshot);
    assert(tree.history_size() == snapshot && tree.can_undo());
    assert(tree.all_prod() == 5 && tree.get(7) == 0);
    tree.undo();
    assert(tree.all_prod() == 0 && tree.nodes_used() == 0 && tree.changes_used() == 0);
    assert(tree.history_size() == 0 && !tree.can_undo());
    RollbackDynamicSegtree<rollback_sum_monoid, 8, 3, 20> small;
    bool thrown = false;
    try{ small.set(2, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && small.history_size() == 0 && small.nodes_used() == 0 && small.changes_used() == 0);
    assert(small.snapshot() == 0 && !small.can_undo());
    RollbackDynamicSegtree<rollback_sum_monoid, 2, 3, 2> history_limited;
    history_limited.set(0, 6);
    thrown = false;
    try{ history_limited.set(1, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && history_limited.history_size() == 1 && history_limited.all_prod() == 6);
    assert(history_limited.snapshot() == 1 && history_limited.can_undo());
    history_limited.undo();
    assert(history_limited.history_size() == 0 && history_limited.nodes_used() == 0 && history_limited.changes_used() == 0);
    assert(!history_limited.can_undo());
}
