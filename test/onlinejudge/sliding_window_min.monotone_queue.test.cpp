// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DSL_3_D

#include <iostream>
#include <vector>

#include "../../src/structure/other/monotone_queue.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, window;
    std::cin >> n >> window;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    auto answer = sliding_window_min<int, 1000000>(values, window);
    for(int i = 0; i < static_cast<int>(answer.size()); i++){
        if(i) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
