// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    assert((number_of_distinct_substrings_by_first_byte("") ==
        std::array<long long, 256>{}));

    const auto aaa = number_of_distinct_substrings_by_first_byte("aaa");
    assert(aaa[static_cast<unsigned char>('a')] == 3);

    const auto abc = number_of_distinct_substrings_by_first_byte("abc");
    assert(abc[static_cast<unsigned char>('a')] == 3);
    assert(abc[static_cast<unsigned char>('b')] == 2);
    assert(abc[static_cast<unsigned char>('c')] == 1);

    const std::string bytes("\xff\0\xff", 3);
    const auto byte_count = number_of_distinct_substrings_by_first_byte(bytes);
    assert(byte_count[0] == 2);
    assert(byte_count[255] == 3);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        const auto count = number_of_distinct_substrings_by_first_byte(text);
        for(int letter = 0; letter < 26; letter++){
            if(letter != 0) std::cout << ' ';
            std::cout << count[static_cast<unsigned char>('a' + letter)];
        }
        std::cout << '\n';
    }
}
