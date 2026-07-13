// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/aho_corasick

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/aho_corasick.hpp"

static AhoCorasick<26, 1000001> automaton;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int string_count;
    std::cin >> string_count;
    std::vector<int> terminal(static_cast<std::size_t>(string_count));
    for(int index = 0; index < string_count; ++index){
        std::string value;
        std::cin >> value;
        terminal[static_cast<std::size_t>(index)] = automaton.add(value);
    }
    automaton.build();

    const int nodes = automaton.node_count();
    std::cout << nodes << '\n';
    for(int vertex = 1; vertex < nodes; ++vertex){
        std::cout << automaton.parent(vertex) << ' '
                  << automaton.failure_link(vertex) << '\n';
    }
    for(int index = 0; index < string_count; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << terminal[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
