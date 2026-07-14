// competitive-verifier: PROBLEM https://www.spoj.com/problems/ANGELS/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/matching/maximum_nonseeing_fighters.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::string> rooms(
            static_cast<std::size_t>(row_count),
            std::string(static_cast<std::size_t>(column_count), 'H')
        );
        for(auto& row: rooms){
            for(char& room: row) std::cin >> room;
        }
        std::cout << maximum_nonseeing_fighters(rooms) << '\n';
    }
}
