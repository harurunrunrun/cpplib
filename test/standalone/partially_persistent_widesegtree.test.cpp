// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/segtree/partially_persistent_widesegtree.hpp"

using Tree = PartiallyPersistentWideSegtree<1000000000039LL, 10000, 256, 12000>;
int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                long long index, value; std::cin >> index >> value;
                std::cout << (type == 'S' ? tree.set(index, value) : tree.add(index, value)) << '\n';
            }else if(type == 'G'){
                int version; long long index; std::cin >> version >> index; std::cout << tree.get(version, index) << '\n';
            }else if(type == 'P'){
                int version; long long left, right; std::cin >> version >> left >> right; std::cout << tree.prod(version, left, right) << '\n';
            }else{
                int version; std::cin >> version; std::cout << tree.all_prod(version) << '\n';
            }
        }
        return 0;
    }
    Tree tree;
    int v1 = tree.add(9, 3);
    int v2 = tree.add(9, 4);
    int v3 = tree.set(1000000000000LL, 5);
    assert(tree.get(v1, 9) == 3 && tree.get(v2, 9) == 7 && tree.all_prod(v3) == 12);
}
