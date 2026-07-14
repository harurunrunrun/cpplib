// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS4/

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

#include "../../src/algorithm/math/basic_number_theory.hpp"
#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int case_number = 0;
    while(std::cin >> size){
        std::vector<long long> values(static_cast<std::size_t>(size));
        std::set<int> unstable;
        for(int index = 0; index < size; ++index){
            std::cin >> values[static_cast<std::size_t>(index)];
            if(values[static_cast<std::size_t>(index)] > 1){
                unstable.insert(index);
            }
        }
        Segtree<AddMonoid<long long>{}, 100000> tree(values);

        std::cout << "Case #" << ++case_number << ":\n";
        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- != 0){
            int operation, left, right;
            std::cin >> operation >> left >> right;
            if(right < left) std::swap(left, right);
            --left;

            if(operation == 1){
                std::cout << tree.prod(left, right) << '\n';
                continue;
            }

            auto iterator = unstable.lower_bound(left);
            while(iterator != unstable.end() && *iterator < right){
                const int index = *iterator;
                const long long value = math::isqrt(tree.get(index));
                tree.set(index, value);
                if(value <= 1){
                    iterator = unstable.erase(iterator);
                }else{
                    ++iterator;
                }
            }
        }
        std::cout << '\n';
    }
}
