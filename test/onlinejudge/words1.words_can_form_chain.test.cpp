// competitive-verifier: PROBLEM https://www.spoj.com/problems/WORDS1/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/constraints/can_order_strings_by_matching_endpoints.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int word_count;
        std::cin >> word_count;
        std::vector<std::string> words(static_cast<std::size_t>(word_count));
        for(auto& word: words) std::cin >> word;
        std::cout << (can_order_strings_by_matching_endpoints(words)
            ? "Ordering is possible.\n"
            : "The door cannot be opened.\n");
    }
}
