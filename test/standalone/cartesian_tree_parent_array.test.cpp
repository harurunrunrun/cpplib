// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/construction/cartesian_tree_parent_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    if(!(std::cin >> size)){
        assert(cartesian_tree_parent_array(std::vector<int>{}).empty());
        assert((cartesian_tree_parent_array(std::vector<int>{3, 1, 2}) ==
            std::vector<int>{1, 1, 1}));
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    const auto answer = cartesian_tree_parent_array(values);
    for(int index = 0; index < size; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
