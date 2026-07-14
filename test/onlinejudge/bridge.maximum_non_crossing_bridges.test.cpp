// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRIDGE/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/maximum_non_crossing_bridges.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<int> first(static_cast<std::size_t>(size));
        std::vector<int> second(static_cast<std::size_t>(size));
        for(int& value: first) std::cin >> value;
        for(int& value: second) std::cin >> value;
        std::cout << maximum_non_crossing_bridges(first, second) << '\n';
    }
}
