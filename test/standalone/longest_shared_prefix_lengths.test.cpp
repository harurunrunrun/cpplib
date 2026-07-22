// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/suffix/longest_shared_prefix_lengths.hpp"

void self_test(){
    assert(longest_shared_prefix_lengths({}).empty());
    assert((longest_shared_prefix_lengths({"alone"}) == std::vector<std::size_t>{0}));
    assert((longest_shared_prefix_lengths({"abc", "abd", "abc", "z"})
        == std::vector<std::size_t>{3, 2, 3, 0}));
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    if(!(std::cin >> n)){
        self_test();
        return 0;
    }
    std::vector<std::string> strings(static_cast<std::size_t>(n));
    for(std::string& value: strings) std::cin >> value;
    for(const std::size_t length: longest_shared_prefix_lengths(strings)){
        std::cout << length << '\n';
    }
}
