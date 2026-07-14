// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../../src/algorithm/string/minimum_cyclic_rotation.hpp"

namespace{

template<class Function>
void expect_invalid_argument(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    assert(minimum_cyclic_rotation_index(std::string_view()) == 0);
    assert(minimum_cyclic_rotation_index(std::string_view("a")) == 0);
    assert(minimum_cyclic_rotation_index(std::string_view("aaaa")) == 0);
    assert(minimum_cyclic_rotation_index(std::string_view("abab")) == 0);
    assert(minimum_cyclic_rotation_index(std::string_view("baba")) == 1);
    assert(minimum_cyclic_rotation_index(std::string_view("aaabaaa")) == 4);

    const std::vector<int> values = {1, 3, 2, 1, 3, 2};
    assert(minimum_cyclic_rotation_index(values.begin(), values.end()) == 0);
    assert(minimum_cyclic_rotation_index(
        values.begin(), values.end(), std::greater<int>{}
    ) == 1);
    expect_invalid_argument([&]{
        (void)minimum_cyclic_rotation_index(values.end(), values.begin());
    });
}

}  // namespace

int main(){
    self_test();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        std::string text;
        std::cin >> text;
        if(text == "-") text.clear();
        std::cout << minimum_cyclic_rotation_index(text) << '\n';
    }
}
