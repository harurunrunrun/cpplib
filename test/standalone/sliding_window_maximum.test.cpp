// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/range/sliding_window_maximum.hpp"

int main(){
    assert((sliding_window_maximum(std::vector<int>{4, 2, 2, 5, 1}, 3) ==
        std::vector<int>{4, 5, 5}));
    assert((sliding_window_maximum(std::vector<int>{-3}, 1) ==
        std::vector<int>{-3}));

    for(int window: {0, 4}){
        bool thrown = false;
        try{
            (void)sliding_window_maximum(std::vector<int>{1, 2, 3}, window);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    int size;
    if(!(std::cin >> size)) return 0;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    int window;
    std::cin >> window;
    const std::vector<int> answer = sliding_window_maximum(values, window);
    for(int index = 0; index < static_cast<int>(answer.size()); index++){
        if(index != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
