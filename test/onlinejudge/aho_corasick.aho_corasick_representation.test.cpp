// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/aho_corasick

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/automata/aho_corasick_representation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int string_count;
    std::cin >> string_count;
    std::vector<std::string> patterns(static_cast<std::size_t>(string_count));
    for(std::string& pattern: patterns) std::cin >> pattern;

    const auto answer = aho_corasick_representation<26, 1000001>(patterns);
    std::cout << answer.node_count() << '\n';
    for(int vertex = 1; vertex < answer.node_count(); ++vertex){
        std::cout << answer.parent[static_cast<std::size_t>(vertex)] << ' '
                  << answer.failure_link[static_cast<std::size_t>(vertex)]
                  << '\n';
    }
    for(int index = 0; index < string_count; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer.terminal_state[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
