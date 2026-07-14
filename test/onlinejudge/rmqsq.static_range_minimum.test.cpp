// competitive-verifier: PROBLEM https://www.spoj.com/problems/RMQSQ/

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_minimum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;

    StaticRangeMinimum<int, 100000> minimum(values);
    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        if(right < left) std::swap(left, right);
        std::cout << minimum.minimum(left, right + 1) << '\n';
    }
}
