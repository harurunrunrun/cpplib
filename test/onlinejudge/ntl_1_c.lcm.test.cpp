#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/NTL_1_C"

#include <iostream>

#include "../../src/algorithm/math/basic_number_theory.hpp"

int main(){
    int count;
    std::cin >> count;
    long long answer = 1;
    while(count--){
        long long value;
        std::cin >> value;
        answer = math::lcm(answer, value);
    }
    std::cout << answer << '\n';
}
