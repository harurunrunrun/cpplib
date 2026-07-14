// competitive-verifier: PROBLEM https://www.spoj.com/problems/ARRAYSUB/

#include <iostream>
#include <vector>

#include "../../src/algorithm/range/sliding_window_maximum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    int window;
    std::cin >> window;

    const std::vector<int> answer = sliding_window_maximum(values, window);
    for(int index = 0; index < static_cast<int>(answer.size()); index++){
        if(index != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
