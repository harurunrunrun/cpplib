// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/automata/aho_corasick_representation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int count;
    if(!(std::cin >> count)){
        const auto result = aho_corasick_representation<3, 32>(
            std::vector<std::string>{"ab", "b", "ab"}
        );
        assert(result.node_count() == 4);
        assert(result.terminal_state[0] == result.terminal_state[2]);
        assert(result.failure_link[static_cast<std::size_t>(
            result.terminal_state[0]
        )] == result.terminal_state[1]);
        return 0;
    }
    std::vector<std::string> patterns(static_cast<std::size_t>(count));
    for(std::string& pattern: patterns) std::cin >> pattern;
    const auto result = aho_corasick_representation<3, 512>(patterns);
    std::cout << result.node_count() << '\n';
    for(int node = 1; node < result.node_count(); ++node){
        std::cout << result.parent[static_cast<std::size_t>(node)] << ' '
                  << result.failure_link[static_cast<std::size_t>(node)] << '\n';
    }
    for(std::size_t index = 0; index < result.terminal_state.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result.terminal_state[index];
    }
    std::cout << '\n';
}
