// competitive-verifier: PROBLEM https://www.spoj.com/problems/ARRAYSUB/

#include <iostream>
#include <vector>

#include "../../src/structure/other/monotone_queue.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    int window;
    std::cin >> window;

    const auto answer = sliding_window_max<int, 100000>(values, window);
    for(int i = 0; i < static_cast<int>(answer.size()); i++){
        if(i != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
