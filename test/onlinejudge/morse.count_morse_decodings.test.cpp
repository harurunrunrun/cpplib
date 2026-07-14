// competitive-verifier: PROBLEM https://www.spoj.com/problems/MORSE/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/count_morse_decodings.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int dataset_count;
    std::cin >> dataset_count;
    while(dataset_count-- > 0){
        std::string signal;
        int word_count;
        std::cin >> signal >> word_count;
        std::vector<std::string> words(static_cast<std::size_t>(word_count));
        for(auto& word: words) std::cin >> word;
        std::cout << count_morse_decodings(
            signal, words, 2'000'000'001LL
        ) << '\n';
    }
}
