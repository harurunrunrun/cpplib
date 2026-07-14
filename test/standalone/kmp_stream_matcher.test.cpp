// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/kmp_stream_matcher.hpp"

int main(){
    KmpStreamMatcher matcher(std::string("aa"));
    assert(matcher.pattern_size() == 2);
    assert(matcher.feed('a') == -1);
    assert(matcher.feed('a') == 0);
    assert(matcher.feed('a') == 1);
    assert(matcher.processed_size() == 3);
    assert(matcher.matched_length() == 1);
    matcher.reset();
    assert(matcher.processed_size() == 0);
    assert(matcher.matched_length() == 0);
    assert(matcher.feed('a') == -1);

    KmpStreamMatcher integer_matcher(std::vector<int>{1, 2, 1});
    assert(integer_matcher.feed(1) == -1);
    assert(integer_matcher.feed(2) == -1);
    assert(integer_matcher.feed(1) == 0);
    assert(integer_matcher.feed(2) == -1);
    assert(integer_matcher.feed(1) == 2);

    bool thrown = false;
    try{
        KmpStreamMatcher empty(std::string{});
        (void)empty;
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        std::string pattern, text;
        std::cin >> pattern >> text;
        KmpStreamMatcher current(std::move(pattern));
        std::vector<long long> positions;
        for(char c: text){
            const long long position = current.feed(c);
            if(position != -1) positions.push_back(position);
        }
        std::cout << positions.size();
        for(long long position: positions) std::cout << ' ' << position;
        std::cout << '\n';
    }
}
