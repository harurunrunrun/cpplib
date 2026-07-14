// competitive-verifier: PROBLEM https://www.spoj.com/problems/WPUZZLES/

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/aho_corasick.hpp"

namespace{

constexpr int MAX_NODES = 1000001;
constexpr std::array<int, 8> DELTA_ROW = {-1, -1, 0, 1, 1, 1, 0, -1};
constexpr std::array<int, 8> DELTA_COLUMN = {0, 1, 1, 1, 0, -1, -1, -1};

AhoCorasick<26, MAX_NODES, 'A'> automaton;

struct Placement{
    int row = -1;
    int column = -1;
    int direction = -1;
};

bool inside(int row, int column, int row_count, int column_count){
    return 0 <= row && row < row_count && 0 <= column && column < column_count;
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int test_index = 0; test_index < test_count; test_index++){
        int row_count, column_count, word_count;
        std::cin >> row_count >> column_count >> word_count;
        std::vector<std::string> grid(static_cast<std::size_t>(row_count));
        for(std::string& row: grid) std::cin >> row;

        automaton.clear();
        std::vector<int> terminal(static_cast<std::size_t>(word_count));
        std::vector<int> length(static_cast<std::size_t>(word_count));
        for(int index = 0; index < word_count; index++){
            std::string word;
            std::cin >> word;
            length[static_cast<std::size_t>(index)] = static_cast<int>(word.size());
            terminal[static_cast<std::size_t>(index)] = automaton.add(word);
        }
        automaton.build();

        std::vector<int> first_word(
            static_cast<std::size_t>(automaton.node_count()), -1
        );
        std::vector<int> next_word(static_cast<std::size_t>(word_count), -1);
        for(int index = 0; index < word_count; index++){
            const int state = terminal[static_cast<std::size_t>(index)];
            next_word[static_cast<std::size_t>(index)] =
                first_word[static_cast<std::size_t>(state)];
            first_word[static_cast<std::size_t>(state)] = index;
        }

        std::vector<Placement> answer(static_cast<std::size_t>(word_count));
        for(int direction = 0; direction < 8; direction++){
            const int dr = DELTA_ROW[static_cast<std::size_t>(direction)];
            const int dc = DELTA_COLUMN[static_cast<std::size_t>(direction)];
            for(int start_row = 0; start_row < row_count; start_row++){
                for(int start_column = 0; start_column < column_count; start_column++){
                    if(inside(
                        start_row - dr, start_column - dc, row_count, column_count
                    )) continue;

                    int state = 0;
                    int row = start_row;
                    int column = start_column;
                    while(inside(row, column, row_count, column_count)){
                        state = automaton.next_state(
                            state,
                            grid[static_cast<std::size_t>(row)]
                                [static_cast<std::size_t>(column)]
                        );
                        int output_state = automaton.terminal_count(state) > 0
                            ? state
                            : automaton.output_link(state);
                        while(output_state != -1){
                            for(
                                int index = first_word[static_cast<std::size_t>(output_state)];
                                index != -1;
                                index = next_word[static_cast<std::size_t>(index)]
                            ){
                                Placement& placement = answer[static_cast<std::size_t>(index)];
                                if(placement.direction != -1) continue;
                                const int offset = length[static_cast<std::size_t>(index)] - 1;
                                placement = {
                                    row - offset * dr,
                                    column - offset * dc,
                                    direction,
                                };
                            }
                            output_state = automaton.output_link(output_state);
                        }
                        row += dr;
                        column += dc;
                    }
                }
            }
        }

        if(test_index != 0) std::cout << '\n';
        for(const Placement& placement: answer){
            std::cout << placement.row << ' ' << placement.column << ' '
                      << static_cast<char>('A' + placement.direction) << '\n';
        }
    }
}
