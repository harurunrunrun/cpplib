// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/perfect_rhyme_dictionary.hpp"

template<class Function>
void expect_rhyme_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_api(){
    PerfectRhymeDictionary empty;
    assert(empty.empty());
    assert(empty.size() == 0);
    assert(!empty.perfect_rhyme("anything"));

    PerfectRhymeDictionary dictionary({"perfect", "rhyme", "crime", "time"});
    assert(!dictionary.empty());
    assert(dictionary.size() == 4);
    assert(dictionary.perfect_rhyme("crime") == "time");
    assert(dictionary.perfect_rhyme("rhyme") == "crime");
    assert(dictionary.perfect_rhyme("dime") == "crime");
    assert(dictionary.perfect_rhyme("perfect") == "crime");
    assert(dictionary.perfect_rhyme("zzz") == "crime");

    dictionary.build({"a", "ba", "cba"});
    assert(dictionary.perfect_rhyme("a") == "ba");
    assert(dictionary.perfect_rhyme("ba") == "cba");
    assert(dictionary.perfect_rhyme("cba") == "ba");

    dictionary.build({""});
    assert(!dictionary.perfect_rhyme(""));
    assert(dictionary.perfect_rhyme("x") == "");

    expect_rhyme_error([&]{ dictionary.build({"same", "same"}); });
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
        PerfectRhymeDictionary dictionary(std::move(words));
        while(query_count-- > 0){
            std::string query;
            std::cin >> query;
            const auto answer = dictionary.perfect_rhyme(query);
            std::cout << (answer ? *answer : std::string_view("NONE")) << '\n';
        }
    }
}
