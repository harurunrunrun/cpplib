// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/range/count_dominance_pairs.hpp"

void self_test(){
    assert(count_dominance_pairs(std::vector<int>{}, std::vector<int>{}) == 0);
    assert(count_dominance_pairs(
        std::vector<int>{50, 100, 150},
        std::vector<int>{1, 3, 2}
    ) == 4);
    assert(count_dominance_pairs(
        std::vector<int>{2, 1}, std::vector<int>{1, 2}
    ) == 3);
    assert(count_dominance_pairs(
        std::vector<int>{2, 1}, std::vector<int>{2, 1}
    ) == 2);
    assert(count_dominance_pairs(
        std::vector<int>{7, 7, 7}, std::vector<int>{4, 4, 4}
    ) == 9);

    bool thrown = false;
    try{
        static_cast<void>(count_dominance_pairs(
            std::vector<int>{1}, std::vector<int>{}
        ));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int n;
    if(!(std::cin >> n)) return 0;
    std::vector<long long> first(static_cast<std::size_t>(n));
    std::vector<long long> second(static_cast<std::size_t>(n));
    for(long long& value: first) std::cin >> value;
    for(long long& value: second) std::cin >> value;
    std::cout << count_dominance_pairs(first, second) << '\n';
}
