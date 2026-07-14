// competitive-verifier: PROBLEM https://www.spoj.com/problems/WPUZZLES/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/find_word_puzzle_placements.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int test_index = 0; test_index < test_count; ++test_index){
        int row_count, column_count, word_count;
        std::cin >> row_count >> column_count >> word_count;
        std::vector<std::string> grid(static_cast<std::size_t>(row_count));
        for(auto& row: grid) std::cin >> row;
        std::vector<std::string> words(static_cast<std::size_t>(word_count));
        for(auto& word: words) std::cin >> word;

        const auto answer = find_word_puzzle_placements(grid, words);
        if(test_index != 0) std::cout << '\n';
        for(const auto& placement: answer){
            std::cout << placement.row << ' ' << placement.column << ' '
                      << static_cast<char>('A' + placement.direction) << '\n';
        }
    }
}
