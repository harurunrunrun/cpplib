// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc281/tasks/abc281_e

#include <iostream>
#include <vector>

#include "../../src/algorithm/sequence/sliding_window_k_smallest_sums.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, window_size;
    int k;
    std::cin >> n >> window_size >> k;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;

    const auto answer = sliding_window_k_smallest_sums<long long, long long>(
        values,
        static_cast<std::size_t>(window_size),
        static_cast<std::size_t>(k)
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
