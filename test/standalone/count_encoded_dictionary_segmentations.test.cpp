// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/count_encoded_dictionary_segmentations.hpp"

namespace{

std::string decode(std::string value){
    return value == "-" ? std::string{} : value;
}

} // namespace

int main(){
    assert(count_encoded_dictionary_segmentations(
        "aaaa", {"a", "aa"}
    ) == 5);
    assert(count_encoded_dictionary_segmentations(
        "aa", {"a", "a"}
    ) == 4);
    assert(count_encoded_dictionary_segmentations(
        "", {"abc"}
    ) == 1);
    assert(count_encoded_dictionary_segmentations(
        "", {"abc"}, 0
    ) == 0);

    bool threw = false;
    try{
        (void)count_encoded_dictionary_segmentations("a", {""});
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    for(int index = 0; index < case_count; ++index){
        std::string text;
        int token_count;
        long long count_limit;
        std::cin >> text >> token_count >> count_limit;
        text = decode(std::move(text));
        std::vector<std::string> tokens(
            static_cast<std::size_t>(token_count)
        );
        for(auto& token: tokens) std::cin >> token;
        std::cout << count_encoded_dictionary_segmentations(
            text, tokens, count_limit
        ) << '\n';
    }
}
