// competitive-verifier: PROBLEM https://www.spoj.com/problems/TRIPINV/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value : values){
        std::cin >> value;
    }

    auto left = std::make_unique<FenwickTree<int, 100000>>(size);
    auto right = std::make_unique<FenwickTree<int, 100000>>(size);
    for(const int value : values){
        right->add(value - 1, 1);
    }

    long long answer = 0;
    for(int index = 0; index < size; index++){
        const int value = values[index];
        right->add(value - 1, -1);
        const int greater_on_left = index - left->prefix_sum(value);
        const int smaller_on_right = right->prefix_sum(value - 1);
        answer += static_cast<long long>(greater_on_left) * smaller_on_right;
        left->add(value - 1, 1);
    }
    std::cout << answer << '\n';
}
