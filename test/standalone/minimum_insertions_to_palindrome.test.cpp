// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/algorithm/string/palindrome/minimum_insertions_to_palindrome.hpp"

int minimum_insertions_dp(const std::string& text){
    std::vector<int> dp(text.size());
    for(std::size_t left = text.size(); left-- > 0;){
        int diagonal = 0;
        for(std::size_t right = left + 1; right < text.size(); right++){
            const int old = dp[right];
            if(text[left] == text[right]){
                dp[right] = diagonal;
            }else{
                dp[right] = 1 + std::min(dp[right], dp[right - 1]);
            }
            diagonal = old;
        }
    }
    return dp.empty() ? 0 : dp.back();
}

std::string random_bytes(std::mt19937& rng, int size){
    std::string result;
    result.reserve(static_cast<std::size_t>(size));
    for(int index = 0; index < size; index++){
        result.push_back(static_cast<char>(rng() & 255U));
    }
    return result;
}

int hex_digit(char value){
    if('0' <= value && value <= '9') return value - '0';
    if('a' <= value && value <= 'f') return value - 'a' + 10;
    assert(false);
    return 0;
}

std::string decode_hex(const std::string& encoded){
    if(encoded == "-") return {};
    assert(encoded.size() % 2 == 0);
    std::string result;
    result.reserve(encoded.size() / 2);
    for(std::size_t index = 0; index < encoded.size(); index += 2){
        result.push_back(static_cast<char>(
            hex_digit(encoded[index]) * 16 + hex_digit(encoded[index + 1])
        ));
    }
    return result;
}

int main(){
    assert(minimum_insertions_to_palindrome(std::string()) == 0);
    assert(minimum_insertions_to_palindrome(std::string("a")) == 0);
    assert(minimum_insertions_to_palindrome(std::string("abba")) == 0);
    assert(minimum_insertions_to_palindrome(std::string("fft")) == 1);
    assert(minimum_insertions_to_palindrome(std::string("abc")) == 2);
    assert(minimum_insertions_to_palindrome(std::string("Aa")) == 1);
    assert(minimum_insertions_to_palindrome(std::vector<int>{1, 2, 1}) == 0);
    assert(minimum_insertions_to_palindrome(std::vector<int>{1, 2, 3}) == 2);

    std::string all_bytes;
    for(int value = 0; value < 256; value++){
        all_bytes.push_back(static_cast<char>(value));
    }
    assert(minimum_insertions_to_palindrome_bytes(all_bytes) == 255);
    assert(minimum_insertions_to_palindrome(all_bytes) == 255);
    std::string palindrome = all_bytes;
    std::string reversed = all_bytes;
    std::reverse(reversed.begin(), reversed.end());
    palindrome += reversed;
    assert(minimum_insertions_to_palindrome_bytes(palindrome) == 0);

    std::mt19937 rng(2349234);
    for(int size: {0, 1, 2, 62, 63, 64, 65, 66, 127, 128, 129}){
        const std::string text = random_bytes(rng, size);
        const int expected = minimum_insertions_dp(text);
        assert(minimum_insertions_to_palindrome_bytes(text)
            == static_cast<std::size_t>(expected));
        assert(minimum_insertions_to_palindrome(text) == expected);
    }
    for(int step = 0; step < 1000; step++){
        const std::string text = random_bytes(
            rng, static_cast<int>(rng() % 161U)
        );
        const int expected = minimum_insertions_dp(text);
        assert(minimum_insertions_to_palindrome_bytes(text)
            == static_cast<std::size_t>(expected));
        assert(minimum_insertions_to_palindrome(text) == expected);
    }

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string encoded;
        std::cin >> encoded;
        const std::string text = decode_hex(encoded);
        std::cout << minimum_insertions_to_palindrome_bytes(text) << '\n';
    }
}
