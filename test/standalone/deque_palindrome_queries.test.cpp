// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/string/palindrome/deque_palindrome_queries.hpp"

void self_test(){
    DequePalindromeQueries<3, 32, 16> queries;
    bool thrown = false;
    try{
        (void)queries.pop_front();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)queries.pop_back();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    auto result = queries.push_back('a');
    assert(result.distinct_palindromes == 1);
    result = queries.push_front('a');
    assert(result.distinct_palindromes == 2);
    assert(result.longest_prefix == 2 && result.longest_suffix == 2);
    result = queries.pop_back();
    assert(result.distinct_palindromes == 1);
    result = queries.pop_front();
    assert(result.distinct_palindromes == 0);
    assert(result.longest_prefix == 0 && result.longest_suffix == 0);
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    DequePalindromeQueries<4, 5002, 5000> queries;
    while(query_count--){
        std::string type;
        std::cin >> type;
        DequePalindromeStatistics result;
        if(type == "PF"){
            char character;
            std::cin >> character;
            result = queries.push_front(character);
        }else if(type == "PB"){
            char character;
            std::cin >> character;
            result = queries.push_back(character);
        }else if(type == "OF"){
            result = queries.pop_front();
        }else{
            result = queries.pop_back();
        }
        std::cout << result.distinct_palindromes << ' '
                  << result.longest_prefix << ' '
                  << result.longest_suffix << '\n';
    }
}
