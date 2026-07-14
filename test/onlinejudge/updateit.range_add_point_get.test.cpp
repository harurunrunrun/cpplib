// competitive-verifier: PROBLEM https://www.spoj.com/problems/UPDATEIT/

#include <iostream>

#include "../../src/structure/other/range_add_point_get.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size, update_count;
        std::cin >> size >> update_count;
        RangeAddPointGet<long long, 10000> values(size);
        while(update_count-- != 0){
            int left, right;
            long long value;
            std::cin >> left >> right >> value;
            values.add(left, right + 1, value);
        }

        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            int index;
            std::cin >> index;
            std::cout << values.get(index) << '\n';
        }
    }
}
