// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/longest_increasing_subsequence.hpp"

void test_basic(){
    const std::vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6};
    const auto indices = longest_increasing_subsequence(values);
    assert((indices == std::vector<int>{1, 2, 4, 7}));
    assert(longest_increasing_subsequence_length(values) == 4);

    assert(longest_increasing_subsequence_length(std::vector<int>{2, 2, 2}, true) == 1);
    assert(longest_increasing_subsequence_length(std::vector<int>{2, 2, 2}, false) == 3);
    assert(longest_increasing_subsequence_length(
        std::vector<int>{1, 3, 2, 0}, true, std::greater<int>()
    ) == 3);
    assert(longest_increasing_subsequence(std::vector<int>{}).empty());
}

int main(){
    int n, strict;
    if(std::cin >> n >> strict){
        std::vector<long long> values(static_cast<std::size_t>(n));
        for(auto& value: values) std::cin >> value;
        std::cout << longest_increasing_subsequence_length(values, strict != 0) << '\n';
        return 0;
    }
    test_basic();
}
