// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc281/tasks/abc281_e

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/structure/other/priority_sum_structure.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, window_size;
    std::size_t k;
    std::cin >> n >> window_size >> k;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;

    std::vector<long long> initial(
        values.begin(), values.begin() + static_cast<std::ptrdiff_t>(window_size)
    );
    PrioritySumStructure<long long, long long> structure(initial, k);
    for(int right = window_size; ; ++right){
        if(right != window_size) std::cout << ' ';
        std::cout << structure.smallest_sum();
        if(right == n) break;
        structure.insert(values[static_cast<std::size_t>(right)]);
        structure.erase(values[static_cast<std::size_t>(right - window_size)]);
    }
    std::cout << '\n';
}
