// competitive-verifier: PROBLEM https://www.spoj.com/problems/KGSS/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/two_maximum_segtree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    using Tree = TwoMaximumSegtree<long long, 100000>;
    auto tree = std::make_unique<Tree>(values);

    int operation_count;
    std::cin >> operation_count;
    while(operation_count-- != 0){
        char operation;
        int left;
        long long right;
        std::cin >> operation >> left >> right;
        if(operation == 'U'){
            tree->set(left - 1, right);
            continue;
        }

        std::cout << tree->sum_of_two_largest(
            left - 1,
            static_cast<int>(right)
        ) << '\n';
    }
}
