// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/sequence/lyndon_factorization.hpp"

void self_test(){
    const std::vector<int> values = {1, 2, 1, 2, 0};
    int comparisons = 0;
    const auto descending = lyndon_factorization(
        values.begin(),
        values.end(),
        [&](int left, int right){
            ++comparisons;
            return left > right;
        }
    );
    assert(comparisons > 0);
    assert(!descending.empty());
    assert(descending.front() == 0);
    assert(descending.back() == values.size());

    const auto empty = lyndon_factorization(
        values.begin(), values.begin(), std::less<int>{}
    );
    assert((empty == std::vector<std::size_t>{0}));
    assert((lyndon_factorization(std::string_view("ababbab")) ==
        std::vector<std::size_t>{0, 5, 7}));

    bool thrown = false;
    try{
        (void)lyndon_factorization(
            values.end(), values.begin(), std::less<int>{}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::string text;
        std::cin >> text;
        if(text == "-") text.clear();
        const std::vector<std::size_t> boundaries =
            lyndon_factorization(text);
        for(std::size_t index = 0; index < boundaries.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << boundaries[index];
        }
        std::cout << '\n';
    }
}
