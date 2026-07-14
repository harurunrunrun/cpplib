// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/string/longest_common_substring_length.hpp"

namespace{

int hexadecimal_digit(char character){
    if('0' <= character && character <= '9') return character - '0';
    if('a' <= character && character <= 'f') return character - 'a' + 10;
    if('A' <= character && character <= 'F') return character - 'A' + 10;
    throw std::runtime_error("invalid hexadecimal digit");
}

std::string decode(std::string encoded){
    if(encoded == "-") return {};
    if(encoded.size() % 2 != 0) throw std::runtime_error("invalid hexadecimal string");
    std::string result(encoded.size() / 2, '\0');
    for(std::size_t index = 0; index < result.size(); index++){
        result[index] = static_cast<char>(
            hexadecimal_digit(encoded[index * 2]) * 16
            + hexadecimal_digit(encoded[index * 2 + 1])
        );
    }
    return result;
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        std::string first, second;
        std::cin >> first >> second;
        std::cout << longest_common_substring_length(decode(first), decode(second)) << '\n';
    }
}
