// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/suffix/longest_common_suffix_dictionary.hpp"

template<class Function>
void expect_dictionary_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void test_api(){
    LongestCommonSuffixDictionary empty;
    assert(empty.empty());
    assert(empty.size() == 0);
    assert(!empty.best_match("anything"));

    LongestCommonSuffixDictionary dictionary({"perfect", "rhyme", "crime", "time"});
    assert(!dictionary.empty());
    assert(dictionary.size() == 4);
    assert(dictionary.best_match("crime") == "time");
    assert(dictionary.best_match("rhyme") == "crime");
    assert(dictionary.best_match("dime") == "crime");
    assert(dictionary.best_match("perfect") == "crime");
    assert(dictionary.best_match("zzz") == "crime");

    dictionary.build({"a", "ba", "cba"});
    assert(dictionary.best_match("a") == "ba");
    assert(dictionary.best_match("ba") == "cba");
    assert(dictionary.best_match("cba") == "ba");

    dictionary.build({""});
    assert(!dictionary.best_match(""));
    assert(dictionary.best_match("x") == "");

    expect_dictionary_error([&]{ dictionary.build({"same", "same"}); });
}

int main(){
    test_api();

    int dataset_count;
    if(!(std::cin >> dataset_count)) return 0;
    while(dataset_count-- > 0){
        int dictionary_size, query_count;
        std::cin >> dictionary_size >> query_count;
        std::vector<std::string> words(dictionary_size);
        for(std::string& word: words) std::cin >> word;
        LongestCommonSuffixDictionary dictionary(std::move(words));
        while(query_count-- > 0){
            std::string query;
            std::cin >> query;
            const auto answer = dictionary.best_match(query);
            std::cout << (answer ? *answer : std::string_view("NONE")) << '\n';
        }
    }
}
