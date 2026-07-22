// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/sequence/shortest_uncommon_subsequence.hpp"

int main(){
    assert(shortest_uncommon_subsequence_length(
        std::vector<int>{1, 1}, std::vector<int>{1}
    ) == 2);
    assert(shortest_uncommon_subsequence_length(
        std::vector<int>{1, 2}, std::vector<int>{1, 2}
    ) == -1);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count--){
        std::string source, forbidden;
        std::cin >> source >> forbidden;
        if(source == "-") source.clear();
        if(forbidden == "-") forbidden.clear();
        std::cout << shortest_uncommon_subsequence_length(source, forbidden) << '\n';
    }
}
