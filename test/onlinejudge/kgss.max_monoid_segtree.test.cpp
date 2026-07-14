#define PROBLEM "https://www.spoj.com/problems/KGSS/"

#include <iostream>
#include <limits>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr MaxMonoid<long long> maximum_monoid{};

bool less_than(long long current, long long target){
    return current < target;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    Segtree<maximum_monoid, 100000> tree(values);

    int operation_count;
    std::cin >> operation_count;
    while(operation_count-- != 0){
        char operation;
        int left;
        long long right;
        std::cin >> operation >> left >> right;
        if(operation == 'U'){
            tree.set(left - 1, right);
            continue;
        }

        --left;
        const int right_index = static_cast<int>(right);
        const long long first_maximum = tree.prod(left, right_index);
        const int first_position = tree.max_right(
            left,
            less_than,
            first_maximum
        );
        tree.set(first_position, std::numeric_limits<long long>::lowest());
        const long long second_maximum = tree.prod(left, right_index);
        tree.set(first_position, first_maximum);
        std::cout << first_maximum + second_maximum << '\n';
    }
}
