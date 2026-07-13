// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc287/tasks/abc287_e

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../../src/structure/trie/trie.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::string> strings(static_cast<std::size_t>(n));
    static Trie<26, 500001> trie;
    for(std::string& s: strings){
        std::cin >> s;
        trie.insert(s);
    }
    for(const std::string& s: strings){
        int lower = 0;
        int upper = static_cast<int>(s.size()) + 1;
        while(upper - lower > 1){
            const int middle = (lower + upper) / 2;
            const std::string_view prefix(s.data(), static_cast<std::size_t>(middle));
            if(trie.prefix_count(prefix) >= 2){
                lower = middle;
            }else{
                upper = middle;
            }
        }
        std::cout << lower << '\n';
    }
}
