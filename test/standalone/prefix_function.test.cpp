// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/automata/prefix_function.hpp"

void test_basic(){
    assert((prefix_function(std::string("ababa")) == std::vector<int>{0, 0, 1, 2, 3}));
    assert((kmp_search(std::string("aaaaa"), std::string("aa")) ==
        std::vector<int>{0, 1, 2, 3}));
    assert((kmp_search(std::string("abc"), std::string("")) ==
        std::vector<int>{0, 1, 2, 3}));
    assert(kmp_search(std::string("abc"), std::string("z")).empty());
    assert((prefix_function(std::vector<int>{1, 2, 1, 2, 1}) ==
        std::vector<int>{0, 0, 1, 2, 3}));
}

int main(){
    std::string text, pattern;
    if(std::cin >> text >> pattern){
        for(int position: kmp_search(text, pattern)){
            std::cout << position << '\n';
        }
        return 0;
    }
    test_basic();
}
