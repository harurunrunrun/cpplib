// competitive-verifier: PROBLEM https://www.spoj.com/problems/MORSE/

#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../../src/algorithm/string/count_encoded_dictionary_segmentations.hpp"

namespace{

std::string encode_uppercase_morse(std::string_view text){
    constexpr std::array<std::string_view, 26> CODE = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
        ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
        "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    };
    std::string encoded;
    for(const char character: text){
        encoded += CODE[static_cast<std::size_t>(character - 'A')];
    }
    return encoded;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int dataset_count;
    std::cin >> dataset_count;
    while(dataset_count-- > 0){
        std::string signal;
        int word_count;
        std::cin >> signal >> word_count;
        std::vector<std::string> encoded_words;
        encoded_words.reserve(static_cast<std::size_t>(word_count));
        while(word_count-- > 0){
            std::string word;
            std::cin >> word;
            encoded_words.push_back(encode_uppercase_morse(word));
        }
        std::cout << count_encoded_dictionary_segmentations(
            signal, encoded_words, 2'000'000'001LL
        ) << '\n';
    }
}
