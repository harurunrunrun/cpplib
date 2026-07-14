// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/segtree/rollback_widesegtree.hpp"

using Tree = RollbackWideSegtree<1000000000039LL, 10000, 12000>;
int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                long long index, value; std::cin >> index >> value;
                if(type == 'S') tree.set(index, value); else tree.add(index, value);
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
    assert(tree.nodes_used() == 0 && !tree.can_undo());
    tree.add(9, 3);
    int snapshot = tree.snapshot();
    assert(snapshot == 1 && tree.history_size() == 1 && tree.can_undo());
    tree.add(9, 4);
    tree.set(1000000000000LL, 5);
    assert(tree.history_size() == 3 && tree.snapshot() == 3);
    assert(tree.nodes_used() > 0);
    assert(tree.all_prod() == 12);
    tree.rollback(snapshot);
    assert(tree.history_size() == snapshot && tree.can_undo());
    assert(tree.all_prod() == 3 && tree.get(1000000000000LL) == 0);
    tree.undo();
    assert(tree.history_size() == 0 && tree.snapshot() == 0);
    assert(tree.nodes_used() == 0 && !tree.can_undo());
    assert(tree.all_prod() == 0);
}
