// competitive-verifier: PROBLEM https://www.spoj.com/problems/PHONELST/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/is_prefix_free_string_set.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int phone_count;
        std::cin >> phone_count;
        std::vector<std::string> phone_numbers(static_cast<std::size_t>(phone_count));
        for(auto& phone_number: phone_numbers) std::cin >> phone_number;
        std::cout << (is_prefix_free_string_set(phone_numbers) ? "YES" : "NO") << '\n';
    }
}
