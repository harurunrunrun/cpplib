// competitive-verifier: STANDALONE

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/residue_count_range_tree.hpp"

template<int MOD>
void run_case(int size, int operation_count){
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;

    using Tree = ResidueCountRangeTree<long long, MOD, 256>;
    auto tree = std::make_unique<Tree>(values);
    for(int operation = 0; operation < operation_count; ++operation){
        char type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        if(type == 'I'){
            tree->increment(left, right);
        }else{
            long long residue;
            std::cin >> residue;
            std::cout << tree->count_residue(left, right, residue) << '\n';
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int modulus;
    int size;
    int operation_count;
    if(!(std::cin >> modulus >> size >> operation_count)) return 0;
    if(modulus == 1) run_case<1>(size, operation_count);
    else if(modulus == 2) run_case<2>(size, operation_count);
    else if(modulus == 3) run_case<3>(size, operation_count);
    else if(modulus == 5) run_case<5>(size, operation_count);
    else run_case<7>(size, operation_count);
}
