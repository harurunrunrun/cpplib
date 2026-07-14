// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/maximum_disjoint_equal_pair_counts.hpp"

void self_test(){
    const std::vector<int> values{7, 7, 7, -2, -2, 7};
    const std::vector<std::pair<int, int>> ranges{
        {0, 6},
        {1, 5},
        {2, 2},
        {3, 6}
    };
    assert(
        maximum_disjoint_equal_pair_counts(values, ranges)
        == std::vector<int>({3, 2, 0, 1})
    );
    assert(
        maximum_disjoint_equal_pair_counts(
            std::vector<std::string>{"b", "a", "b", "b"},
            std::vector<std::pair<int, int>>{{0, 4}}
        ) == std::vector<int>({1})
    );

    bool thrown = false;
    try{
        (void)maximum_disjoint_equal_pair_counts(
            values,
            std::vector<std::pair<int, int>>{{-1, 2}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int n, query_count;
    if(!(std::cin >> n >> query_count)) return 0;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    std::vector<std::pair<int, int>> ranges(static_cast<std::size_t>(query_count));
    for(auto& [left, right]: ranges) std::cin >> left >> right;

    for(int answer: maximum_disjoint_equal_pair_counts(values, ranges)){
        std::cout << answer << '\n';
    }
}
