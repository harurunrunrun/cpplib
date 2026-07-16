// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/string/maximum_consecutive_repetitions.hpp"

namespace{

int hexadecimal_digit(char digit){
    if('0' <= digit && digit <= '9') return digit - '0';
    if('a' <= digit && digit <= 'f') return digit - 'a' + 10;
    throw std::runtime_error("invalid hexadecimal test input");
}

std::string decode_hexadecimal(const std::string& encoded){
    if(encoded == "-") return {};
    if(encoded.size() % 2 != 0){
        throw std::runtime_error("odd hexadecimal test input");
    }
    std::string text(encoded.size() / 2, '\0');
    for(std::size_t index = 0; index < text.size(); ++index){
        text[index] = static_cast<char>(
            hexadecimal_digit(encoded[2 * index]) * 16 +
            hexadecimal_digit(encoded[2 * index + 1])
        );
    }
    return text;
}

} // namespace

int main(){
    assert(maximum_consecutive_repetitions("") == 0);
    assert(maximum_consecutive_repetitions("a") == 1);
    assert(maximum_consecutive_repetitions("aaaaa") == 5);
    assert(maximum_consecutive_repetitions("abababab") == 4);
    assert(maximum_consecutive_repetitions("xababab") == 3);
    assert(maximum_consecutive_repetitions("abcabca") == 2);
    assert(maximum_consecutive_repetitions("abcdef") == 1);
    assert(maximum_consecutive_repetitions("zabcabcabcx") == 3);
    assert(maximum_consecutive_repetitions("aabaabaa") == 2);

    std::string all_bytes;
    for(int value = 0; value < 256; ++value){
        all_bytes.push_back(static_cast<char>(value));
    }
    all_bytes += all_bytes;
    assert(maximum_consecutive_repetitions(all_bytes) == 2);
    const std::string binary_seed{"\0\xff\x7f\x80", 4};
    assert(maximum_consecutive_repetitions(
        binary_seed + binary_seed + binary_seed
    ) == 3);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        std::string encoded;
        std::cin >> encoded;
        std::cout << maximum_consecutive_repetitions(
            decode_hexadecimal(encoded)
        ) << '\n';
    }
}
