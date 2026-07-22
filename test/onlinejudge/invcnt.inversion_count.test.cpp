// competitive-verifier: PROBLEM https://www.spoj.com/problems/INVCNT/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/sequence/inversion_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<int> values(static_cast<std::size_t>(size));
        for(int& value: values) std::cin >> value;
        std::cout << inversion_count(values) << '\n';
    }
}
