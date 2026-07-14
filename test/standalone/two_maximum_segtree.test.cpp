// competitive-verifier: STANDALONE

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/two_maximum_segtree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int operation_count;
    if(!(std::cin >> size >> operation_count)) return 0;

    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;

    using Tree = TwoMaximumSegtree<long long, 256>;
    auto tree = std::make_unique<Tree>(values);
    for(int operation = 0; operation < operation_count; ++operation){
        char type;
        int first;
        long long second;
        std::cin >> type >> first >> second;
        if(type == 'S'){
            tree->set(first, second);
        }else{
            std::cout << tree->sum_of_two_largest(
                first,
                static_cast<int>(second)
            ) << '\n';
        }
    }
}
