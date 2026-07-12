// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/structure/segtree/persistent_widesegtree.hpp"

using Tree = PersistentWideSegtree<1000000000039LL, 10000, 256>;
int main(){
    int q;
    if(std::cin >> q){
        Tree tree;
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                int version; long long index, value; std::cin >> version >> index >> value;
                std::cout << (type == 'S' ? tree.set(version, index, value) : tree.add(version, index, value)) << '\n';
            }else if(type == 'F'){
                int version; std::cin >> version; std::cout << tree.fork(version) << '\n';
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
    int v1 = tree.add(0, 9, 3);
    int v2 = tree.add(v1, 9, 4);
    int v3 = tree.set(v1, 1000000000000LL, 5);
    assert(tree.get(v2, 9) == 7 && tree.get(v3, 9) == 3 && tree.all_prod(v3) == 8);
}
