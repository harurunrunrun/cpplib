// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS6/

#include <iostream>
#include <vector>

#include "../../src/structure/bbst/reversible_splay_tree.hpp"
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
    ReversibleSplayTree<NonEmptyMaxSubarrayMonoid<long long>{}, 200000> sequence(values);

    int operation_count;
    std::cin >> operation_count;
    while(operation_count-- != 0){
        char operation;
        int position;
        std::cin >> operation >> position;
        if(operation == 'D'){
            sequence.erase(position - 1);
            continue;
        }

        long long argument;
        std::cin >> argument;
        if(operation == 'I'){
            sequence.insert(position - 1, non_empty_max_subarray_singleton(argument));
        }else if(operation == 'R'){
            sequence.set(position - 1, non_empty_max_subarray_singleton(argument));
        }else{
            std::cout << sequence.prod(position - 1, static_cast<int>(argument)).best << '\n';
        }
    }
}
