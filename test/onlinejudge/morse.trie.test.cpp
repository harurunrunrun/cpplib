// competitive-verifier: PROBLEM https://www.spoj.com/problems/MORSE/

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../../src/structure/trie/trie.hpp"

namespace{

constexpr std::array<std::string_view, 26> MORSE_CODE = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
};

constexpr long long COUNT_LIMIT = 2'000'000'001LL;
Trie<2, 800001, '-'> dictionary;

std::string encode_word(const std::string& word){
    std::string encoded;
    encoded.reserve(word.size() * 4);
    for(char character: word){
        encoded.append(MORSE_CODE[static_cast<std::size_t>(character - 'A')]);
    }
    return encoded;
}

long long count_phrases(const std::string& signal, int maximum_word_length){
    std::vector<long long> ways(signal.size() + 1);
    ways[0] = 1;
    for(std::size_t start = 0; start < signal.size(); start++){
        if(ways[start] == 0) continue;
        int state = 0;
        const std::size_t limit = std::min(
            signal.size(),
            start + static_cast<std::size_t>(maximum_word_length)
        );
        for(std::size_t end = start; end < limit; end++){
            state = dictionary.next_node(state, signal[end]);
            if(state == -1) break;
            const int multiplicity = dictionary.terminal_count(state);
            if(multiplicity == 0) continue;
            const long long addition = ways[start] * multiplicity;
            ways[end + 1] = std::min(
                COUNT_LIMIT,
                ways[end + 1] + addition
            );
        }
    }
    return ways.back();
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int dataset_count;
    std::cin >> dataset_count;
    while(dataset_count-- > 0){
        std::string signal;
        int word_count;
        std::cin >> signal >> word_count;

        dictionary.clear();
        int maximum_word_length = 0;
        while(word_count-- > 0){
            std::string word;
            std::cin >> word;
            std::string encoded = encode_word(word);
            maximum_word_length = std::max(
                maximum_word_length,
                static_cast<int>(encoded.size())
            );
            dictionary.insert(encoded);
        }
        std::cout << count_phrases(signal, maximum_word_length) << '\n';
    }
}
