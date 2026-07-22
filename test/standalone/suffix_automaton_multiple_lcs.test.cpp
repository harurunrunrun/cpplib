// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../../src/algorithm/string/suffix/suffix_automaton.hpp"

int main(){
    SuffixAutomaton<3, 4> empty_automaton;
    const std::array<std::string_view, 1> nonempty_target{"abc"};
    assert(empty_automaton.longest_common_substring_length(nonempty_target) == 0);

    SuffixAutomaton<3, 64> automaton("ababa");
    const std::array<std::string_view, 2> targets{"babab", "cababa"};
    assert(automaton.longest_common_substring_length(targets) == 4);
    assert(automaton.longest_common_substring_length({}) == 5);
    const std::array<std::string_view, 1> empty_target{""};
    assert(automaton.longest_common_substring_length(empty_target) == 0);

    automaton.extend('c');
    const std::array<std::string_view, 1> after_extend{"ccabacc"};
    assert(automaton.longest_common_substring_length(after_extend) == 4);

    bool thrown = false;
    try{
        const std::array<std::string_view, 1> invalid{"d"};
        (void)automaton.longest_common_substring_length(invalid);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int string_count;
        std::cin >> string_count;
        std::vector<std::string> texts(static_cast<std::size_t>(string_count));
        for(std::string& text: texts) std::cin >> text;

        SuffixAutomaton<26, 256> current(texts.front());
        std::vector<std::string_view> additional;
        for(int index = 1; index < string_count; index++){
            additional.push_back(texts[static_cast<std::size_t>(index)]);
        }
        std::cout << current.longest_common_substring_length(additional) << '\n';
    }
}
