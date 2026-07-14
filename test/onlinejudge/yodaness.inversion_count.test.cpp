// competitive-verifier: PROBLEM https://www.spoj.com/problems/YODANESS/

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/inversion_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int word_count;
        std::cin >> word_count;
        std::vector<std::string> yoda(static_cast<std::size_t>(word_count));
        for(std::string& word: yoda) std::cin >> word;

        std::unordered_map<std::string, int> normal_position;
        normal_position.reserve(static_cast<std::size_t>(word_count) * 2);
        normal_position.max_load_factor(0.7F);
        for(int position = 0; position < word_count; ++position){
            std::string word;
            std::cin >> word;
            normal_position.emplace(std::move(word), position);
        }

        std::vector<int> positions;
        positions.reserve(yoda.size());
        for(const std::string& word: yoda){
            positions.push_back(normal_position.at(word));
        }
        std::cout << inversion_count(positions) << '\n';
    }
}
