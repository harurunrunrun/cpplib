// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/palindrome/enumerate_palindrome_lengths.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)){
        assert(enumerate_palindrome_lengths("").empty());
        assert((enumerate_palindrome_lengths("abac") ==
            std::vector<int>{1, 0, 3, 0, 1, 0, 1}));
        return 0;
    }
    const auto answer = enumerate_palindrome_lengths(text);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
