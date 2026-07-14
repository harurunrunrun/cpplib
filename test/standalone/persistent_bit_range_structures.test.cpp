// competitive-verifier: STANDALONE

#include <iostream>
#include <string>

#include "../../src/structure/segtree/detail/persistent_bit_range_tree.hpp"
#include "../../src/structure/segtree/persistent_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/persistent_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/persistent_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/persistent_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/persistent_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/persistent_dynamic_bitoverwrite_rangesum.hpp"

template<class Tree>
void run_assign(int n, int q){
    Tree tree(n);
    while(q--){
        char operation;
        int version, left, right, value;
        std::cin >> operation >> version;
        if(operation == 'A'){
            std::cin >> left >> right >> value;
            std::cout << tree.assign(version, left, right, value != 0) << '\n';
        }else if(operation == 'K'){
            std::cout << tree.fork(version) << '\n';
        }else if(operation == 'Q'){
            std::cin >> left >> right;
            std::cout << tree.sum(version, left, right) << '\n';
        }else{
            std::cin >> left;
            std::cout << tree.get(version, left) << '\n';
        }
    }
}

template<class Tree>
void run_flip(int n, int q){
    Tree tree(n);
    while(q--){
        char operation;
        int version, left, right, value;
        std::cin >> operation >> version;
        if(operation == 'A'){
            std::cin >> left >> right >> value;
            std::cout << tree.assign(version, left, right, value != 0) << '\n';
        }else if(operation == 'F'){
            std::cin >> left >> right;
            std::cout << tree.flip(version, left, right) << '\n';
        }else if(operation == 'K'){
            std::cout << tree.fork(version) << '\n';
        }else if(operation == 'Q'){
            std::cin >> left >> right;
            std::cout << tree.sum(version, left, right) << '\n';
        }else{
            std::cin >> left;
            std::cout << tree.get(version, left) << '\n';
        }
    }
}

template<class Tree>
void run_overwrite(int n, int q){
    Tree tree(n);
    while(q--){
        char operation;
        int version, left, right;
        std::cin >> operation >> version;
        if(operation == 'S'){
            std::cin >> left >> right;
            std::cout << tree.set(version, left, right) << '\n';
        }else if(operation == 'K'){
            std::cout << tree.fork(version) << '\n';
        }else if(operation == 'Q'){
            std::cin >> left >> right;
            std::cout << tree.sum(version, left, right) << '\n';
        }else{
            std::cin >> left;
            std::cout << tree.get(version, left) << '\n';
        }
    }
}

int main(){
    std::string mode;
    int n, q;
    if(!(std::cin >> mode >> n >> q)) return 0;
    constexpr int max_nodes = 200000;
    constexpr int max_versions = 1000;
    if(mode == "ASSIGN"){
        run_assign<PersistentBitAssignRangeSum<2048, max_nodes, max_versions>>(n, q);
    }else if(mode == "DASSIGN"){
        run_assign<PersistentDynamicBitAssignRangeSum<2048, max_nodes, max_versions>>(n, q);
    }else if(mode == "FLIP"){
        run_flip<PersistentBitAssignRangeSumRangeFlip<2048, max_nodes, max_versions>>(n, q);
    }else if(mode == "DFLIP"){
        run_flip<PersistentDynamicBitAssignRangeSumRangeFlip<2048, max_nodes, max_versions>>(n, q);
    }else if(mode == "OVERWRITE"){
        run_overwrite<PersistentBitOverwriteRangeSum<2048, max_nodes, max_versions>>(n, q);
    }else{
        run_overwrite<PersistentDynamicBitOverwriteRangeSum<2048, max_nodes, max_versions>>(n, q);
    }
}
