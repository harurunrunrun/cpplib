// competitive-verifier: PROBLEM https://www.spoj.com/problems/YODANESS/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/other/sequence/relative_order_inversion_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int word_count;
        std::cin >> word_count;
        std::vector<std::string> yoda(static_cast<std::size_t>(word_count));
        for(std::string& word: yoda) std::cin >> word;

        std::vector<std::string> normal(static_cast<std::size_t>(word_count));
        for(std::string& word: normal) std::cin >> word;
        std::cout << relative_order_inversion_count(yoda, normal) << '\n';
    }
}
