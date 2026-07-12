// competitive-verifier: STANDALONE

#include <iostream>
#include <memory>
#include <string>

#include "../../src/structure/segtree/partially_persistent_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/partially_persistent_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/partially_persistent_dynamic_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/rollback_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/rollback_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/rollback_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/rollback_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/rollback_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/rollback_dynamic_bitoverwrite_rangesum.hpp"

template<class Tree>
void run_partial(int n, int q, int kind){
    auto tree = std::make_unique<Tree>(n);
    while(q--){
        char operation;
        int version, left, right, value;
        std::cin >> operation;
        if(operation == 'A'){
            std::cin >> left >> right >> value;
            std::cout << tree->assign(left, right, value != 0) << '\n';
        }else if(operation == 'F'){
            std::cin >> left >> right;
            std::cout << tree->flip(left, right) << '\n';
        }else if(operation == 'S'){
            std::cin >> left >> right;
            std::cout << tree->set(left, right) << '\n';
        }else if(operation == 'K'){
            std::cout << tree->fork(tree->latest_version()) << '\n';
        }else if(operation == 'Q'){
            std::cin >> version >> left >> right;
            std::cout << tree->sum(version, left, right) << '\n';
        }else{
            std::cin >> version >> left;
            std::cout << tree->get(version, left) << '\n';
        }
        (void)kind;
    }
}

template<class Tree>
void run_rollback(int n, int q){
    auto tree = std::make_unique<Tree>(n);
    while(q--){
        char operation;
        int version, left, right, value;
        std::cin >> operation;
        if(operation == 'A'){
            std::cin >> left >> right >> value;
            std::cout << tree->assign(left, right, value != 0) << '\n';
        }else if(operation == 'F'){
            std::cin >> left >> right;
            std::cout << tree->flip(left, right) << '\n';
        }else if(operation == 'S'){
            std::cin >> left >> right;
            std::cout << tree->set(left, right) << '\n';
        }else if(operation == 'K'){
            std::cout << tree->fork() << '\n';
        }else if(operation == 'B'){
            std::cin >> version;
            tree->rollback(version);
            std::cout << tree->latest_version() << '\n';
        }else if(operation == 'Q'){
            std::cin >> left >> right;
            std::cout << tree->sum(left, right) << '\n';
        }else{
            std::cin >> left;
            std::cout << tree->get(left) << '\n';
        }
    }
}

int main(){
    std::string mode;
    int n, q;
    if(!(std::cin >> mode >> n >> q)) return 0;
    constexpr int nodes = 12000;
    constexpr int versions = 1000;
    constexpr int changes = 250000;
    if(mode == "PASSIGN") run_partial<PartiallyPersistentBitAssignRangeSum<2048, nodes, versions, changes>>(n, q, 0);
    else if(mode == "PDASSIGN") run_partial<PartiallyPersistentDynamicBitAssignRangeSum<2048, nodes, versions, changes>>(n, q, 0);
    else if(mode == "PFLIP") run_partial<PartiallyPersistentBitAssignRangeSumRangeFlip<2048, nodes, versions, changes>>(n, q, 1);
    else if(mode == "PDFLIP") run_partial<PartiallyPersistentDynamicBitAssignRangeSumRangeFlip<2048, nodes, versions, changes>>(n, q, 1);
    else if(mode == "POVERWRITE") run_partial<PartiallyPersistentBitOverwriteRangeSum<2048, nodes, versions, changes>>(n, q, 2);
    else if(mode == "PDOVERWRITE") run_partial<PartiallyPersistentDynamicBitOverwriteRangeSum<2048, nodes, versions, changes>>(n, q, 2);
    else if(mode == "RASSIGN") run_rollback<RollbackBitAssignRangeSum<2048, nodes, versions, changes>>(n, q);
    else if(mode == "RDASSIGN") run_rollback<RollbackDynamicBitAssignRangeSum<2048, nodes, versions, changes>>(n, q);
    else if(mode == "RFLIP") run_rollback<RollbackBitAssignRangeSumRangeFlip<2048, nodes, versions, changes>>(n, q);
    else if(mode == "RDFLIP") run_rollback<RollbackDynamicBitAssignRangeSumRangeFlip<2048, nodes, versions, changes>>(n, q);
    else if(mode == "ROVERWRITE") run_rollback<RollbackBitOverwriteRangeSum<2048, nodes, versions, changes>>(n, q);
    else run_rollback<RollbackDynamicBitOverwriteRangeSum<2048, nodes, versions, changes>>(n, q);
}
