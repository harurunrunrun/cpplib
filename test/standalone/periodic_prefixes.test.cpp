// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/automata/periodic_prefixes.hpp"

int main(){
    assert(periodic_prefixes(std::string()).empty());
    assert(periodic_prefixes(std::string("a")).empty());
    assert((periodic_prefixes(std::string("aaa")) ==
        std::vector<std::pair<int, int>>{{2, 2}, {3, 3}}));
    assert((periodic_prefixes(std::string("ababab")) ==
        std::vector<std::pair<int, int>>{{4, 2}, {6, 3}}));
    assert((periodic_prefixes(std::vector<int>{1, 2, 1, 2}) ==
        std::vector<std::pair<int, int>>{{4, 2}}));

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        const auto result = periodic_prefixes(text);
        std::cout << result.size() << '\n';
        for(const auto& [prefix_length, repetition_count]: result){
            std::cout << prefix_length << ' ' << repetition_count << '\n';
        }
    }
}
