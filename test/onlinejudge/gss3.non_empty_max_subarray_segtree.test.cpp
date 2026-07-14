// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS3/

#include <iostream>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<NonEmptyMaxSubarrayAggregate<long long>> values(
        static_cast<std::size_t>(size)
    );
    for(auto& value: values){
        long long input;
        std::cin >> input;
        value = non_empty_max_subarray_singleton(input);
    }
    Segtree<NonEmptyMaxSubarrayMonoid<long long>{}, 50000> tree(values);

    int operation_count;
    std::cin >> operation_count;
    while(operation_count-- != 0){
        int operation, left;
        long long right;
        std::cin >> operation >> left >> right;
        if(operation == 0){
            tree.set(left - 1, non_empty_max_subarray_singleton(right));
        }else{
            std::cout << tree.prod(left - 1, static_cast<int>(right)).best << '\n';
        }
    }
}
