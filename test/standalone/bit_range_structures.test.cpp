// competitive-verifier: STANDALONE

#include <iostream>
#include <string>
#include <vector>

#define Bit01WST BitAssignRangeSumTree
#include "../../src/structure/segtree/bitassign_rangesum.hpp"
#undef Bit01WST
#define Bit01WST BitAssignFlipRangeSumTree
#include "../../src/structure/segtree/bitassign_rangesum_rangeflip.hpp"
#undef Bit01WST
#include "../../src/structure/segtree/bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/dynamic_bitoverwrite_rangesum.hpp"

int main(){
    std::string mode;
    int n, q;
    if(!(std::cin >> mode >> n >> q)) return 0;

    if(mode == "ASSIGN"){
        std::vector<int> initial(static_cast<std::size_t>(n));
        for(int& value: initial) std::cin >> value;
        BitAssignRangeSumTree dense(initial);
        DynamicBit01WSTNoFlip sparse(initial);
        while(q--){
            char operation;
            int l, r, value;
            std::cin >> operation;
            if(operation == 'A'){
                std::cin >> l >> r >> value;
                dense.assign(l, r, value != 0);
                sparse.assign(l, r, value != 0);
            }else if(operation == 'Q'){
                std::cin >> l >> r;
                std::cout << dense.sum(l, r) << ' ' << sparse.sum(l, r) << '\n';
            }else{
                std::cin >> l;
                std::cout << dense.get(l) << ' ' << sparse.get(l) << '\n';
            }
        }
    }else if(mode == "FLIP"){
        std::vector<int> initial(static_cast<std::size_t>(n));
        for(int& value: initial) std::cin >> value;
        BitAssignFlipRangeSumTree dense(initial);
        DynamicBit01WST sparse(initial);
        while(q--){
            char operation;
            int l, r, value;
            std::cin >> operation;
            if(operation == 'A'){
                std::cin >> l >> r >> value;
                dense.assign(l, r, value != 0);
                sparse.assign(l, r, value != 0);
            }else if(operation == 'F'){
                std::cin >> l >> r;
                dense.flip(l, r);
                sparse.flip(l, r);
            }else if(operation == 'Q'){
                std::cin >> l >> r;
                std::cout << dense.sum(l, r) << ' ' << sparse.sum(l, r) << '\n';
            }else{
                std::cin >> l;
                std::cout << dense.get(l) << ' ' << sparse.get(l) << '\n';
            }
        }
    }else{
        WideSetSum<> dense(static_cast<std::size_t>(n));
        DynamicWideSetSum<> sparse(static_cast<std::size_t>(n));
        while(q--){
            char operation;
            int l, r;
            std::cin >> operation;
            if(operation == 'S'){
                std::cin >> l >> r;
                dense.set(l, r);
                sparse.set(l, r);
            }else if(operation == 'Q'){
                std::cin >> l >> r;
                std::cout << dense.sum(l, r) << ' ' << sparse.sum(l, r) << '\n';
            }else{
                std::cout << dense.all_sum() << ' ' << sparse.all_sum() << '\n';
            }
        }
    }
}
