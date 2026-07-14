// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/shortest_palindrome_by_appending.hpp"

int main(){
    assert(shortest_palindrome_by_appending("").empty());
    assert(shortest_palindrome_by_appending("a") == "a");
    assert(shortest_palindrome_by_appending("ab") == "aba");
    assert(shortest_palindrome_by_appending("abba") == "abba");
    assert(shortest_palindrome_by_appending("Aa") == "AaA");
    assert(shortest_palindrome_by_appending("amanaplanacanal") ==
        "amanaplanacanalpanama");

    const std::string with_null("a\0b", 3);
    const std::string expected_with_null("a\0b\0a", 5);
    assert(shortest_palindrome_by_appending(with_null) == expected_with_null);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << shortest_palindrome_by_appending(text) << '\n';
    }
}
