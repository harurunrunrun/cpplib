// competitive-verifier: PROBLEM https://www.spoj.com/problems/RPLN/

#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_minimum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    for(int test = 1; test <= test_count; test++){
        int size, query_count;
        std::cin >> size >> query_count;
        std::vector<int> values(static_cast<std::size_t>(size));
        for(int& value: values) std::cin >> value;
        StaticRangeMinimum<int, 100000> minimum(values);

        std::cout << "Scenario #" << test << ":\n";
        while(query_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            std::cout << minimum.minimum(left - 1, right) << '\n';
        }
    }
}
