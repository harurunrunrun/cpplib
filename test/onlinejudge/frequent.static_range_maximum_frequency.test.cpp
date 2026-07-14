// competitive-verifier: PROBLEM https://www.spoj.com/problems/FREQUENT/

#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_maximum_frequency.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    while(std::cin >> n && n != 0){
        int q;
        std::cin >> q;
        std::vector<int> values(n);
        for(int& value: values) std::cin >> value;
        StaticRangeMaximumFrequency<int, 100000> frequency(values);
        while(q--){
            int left, right;
            std::cin >> left >> right;
            std::cout << frequency.maximum_frequency(left - 1, right) << '\n';
        }
    }
}
