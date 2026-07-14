// competitive-verifier: PROBLEM https://www.spoj.com/problems/PERIOD/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/periodic_prefixes.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int test_case = 1; test_case <= test_count; test_case++){
        int size;
        std::string text;
        std::cin >> size;
        text.reserve(static_cast<std::size_t>(size));
        std::cin >> text;

        std::cout << "Test case #" << test_case << '\n';
        for(const auto& [prefix_length, repetition_count]: periodic_prefixes(text)){
            std::cout << prefix_length << ' ' << repetition_count << '\n';
        }
        std::cout << '\n';
    }
}
