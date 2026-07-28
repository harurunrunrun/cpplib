// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/assignment_subset/dancing_links.hpp"
#include "../../src/algorithm/other/dynamic_programming/assignment_subset/exact_cover.hpp"
#include "../../src/algorithm/other/dynamic_programming/resource/bitset_knapsack.hpp"
#include "../../src/algorithm/other/dynamic_programming/sequence_selection/hirschberg_lcs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            int capacity;
            int item_count;
            std::cin >> capacity >> item_count;
            BitsetKnapsack knapsack(capacity);
            for(int index = 0; index < item_count; ++index){
                int weight;
                std::cin >> weight;
                knapsack.add(weight);
            }
            std::cout << knapsack.maximum_reachable();
            for(int weight = 0; weight <= capacity; ++weight){
                std::cout << static_cast<int>(knapsack.reachable(weight));
            }
            std::cout << '\n';
        }else if(kind == 1){
            std::string first;
            std::string second;
            std::cin >> first >> second;
            std::vector<char> left(first.begin(), first.end());
            std::vector<char> right(second.begin(), second.end());
            auto lcs = hirschberg_lcs(left, right);
            auto is_subsequence = [](const std::vector<char>& subsequence,
                const std::vector<char>& sequence){
                std::size_t position = 0;
                for(char value: sequence){
                    if(position < subsequence.size()
                        && subsequence[position] == value) ++position;
                }
                return position == subsequence.size();
            };
            if(!is_subsequence(lcs, left)
                || !is_subsequence(lcs, right)) return 2;
            std::cout << lcs.size() << '\n';
        }else{
            int column_count;
            int row_count;
            std::cin >> column_count >> row_count;
            std::vector<std::vector<int>> rows(
                static_cast<std::size_t>(row_count)
            );
            DancingLinks solver(column_count);
            for(auto& row: rows){
                int size;
                std::cin >> size;
                row.resize(static_cast<std::size_t>(size));
                for(int& column: row) std::cin >> column;
                solver.add_row(row);
            }
            const auto all = solver.solve(
                static_cast<std::size_t>(1) << 20
            );
            const auto one = exact_cover(column_count, rows);
            if(one.has_value() != !all.empty()) return 3;
            for(const auto& solution: all){
                std::vector<int> covered(
                    static_cast<std::size_t>(column_count), 0
                );
                for(int row_id: solution){
                    for(int column:
                        rows[static_cast<std::size_t>(row_id)]){
                        ++covered[static_cast<std::size_t>(column)];
                    }
                }
                if(std::any_of(
                    covered.begin(), covered.end(),
                    [](int value){ return value != 1; }
                )) return 4;
            }
            std::cout << all.size() << '\n';
        }
    }
}
