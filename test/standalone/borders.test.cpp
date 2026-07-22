// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/automata/borders.hpp"

int main(){
    assert((border_lengths(std::string("ababab")) == std::vector<int>{2, 4}));
    assert((border_lengths(std::string("aaaa")) == std::vector<int>{1, 2, 3}));
    assert(border_lengths(std::string()).empty());
    assert((border_lengths(std::vector<int>{1, 2, 1, 2, 1}) ==
        std::vector<int>{1, 3}));

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count--){
        std::string text;
        std::cin >> text;
        if(text == "-") text.clear();
        const std::vector<int> lengths = border_lengths(text);
        std::cout << lengths.size();
        for(int length: lengths) std::cout << ' ' << length;
        std::cout << '\n';
    }
}
