// competitive-verifier: PROBLEM https://www.spoj.com/problems/CDC12_H/

#include <iostream>
#include <string>

#include "../../src/structure/segtree/range_add_range_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    for(int case_index = 1; case_index <= test_count; case_index++){
        int size, query_count;
        std::cin >> size >> query_count;
        RangeAddRangeSum<long long, 200000> ranges(size);

        std::cout << "Scenario #" << case_index << ":\n";
        while(query_count-- != 0){
            std::string command;
            int left, right;
            std::cin >> command >> left >> right;
            --left;
            if(command == "modification"){
                ranges.add(left, right, 1LL);
                std::cout << "OK\n";
            }else{
                std::cout << ranges.sum(left, right) << '\n';
            }
        }
    }
}
