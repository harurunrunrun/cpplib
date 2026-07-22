// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/sequence/longest_increasing_subsequence.hpp"

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

    const std::vector<int> descending_with_duplicates = {3, 3, 2, 2, 1};
    const auto non_strict = longest_increasing_subsequence(
        descending_with_duplicates, false, std::greater<int>()
    );
    assert((non_strict == std::vector<int>{0, 1, 2, 3, 4}));
    const auto strict = longest_increasing_subsequence(
        descending_with_duplicates, true, std::greater<int>()
    );
    assert(strict.size() == 3);
    for(std::size_t index = 1; index < strict.size(); ++index){
        assert(descending_with_duplicates[strict[index - 1]]
            > descending_with_duplicates[strict[index]]);
    }
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
