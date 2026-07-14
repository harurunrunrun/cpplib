// competitive-verifier: PROBLEM https://www.spoj.com/problems/ANARC05H/
#include <iostream>
#include <string>
#include "../../src/algorithm/string/count_nondecreasing_digit_sum_partitions.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    std::string text; int index=1; while(std::cin>>text && text!="bye")
        std::cout<<index++<<". "<<count_nondecreasing_digit_sum_partitions(text)<<'\n';
}
